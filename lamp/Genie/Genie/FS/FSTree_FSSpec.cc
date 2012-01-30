/*	================
 *	FSTree_FSSpec.cc
 *	================
 */

#include "Genie/FS/FSTree_FSSpec.hh"

// Standard C++
#include <algorithm>

// POSIX
#include "fcntl.h"
#include "sys/stat.h"

// Iota
#include "iota/convert_string.hh"
#include "iota/strings.hh"

// MoreFiles
#include "MoreFiles/FileCopy.h"
#include "MoreFiles/MoreFilesExtras.h"

// Debug
#include "debug/assert.hh"

// plus
#include "plus/mac_utf8.hh"
#include "plus/replaced_string.hh"

// Nitrogen
#include "Nitrogen/Aliases.hh"
#include "Nitrogen/Files.hh"
#include "Nitrogen/Resources.hh"

// Io: MacFiles
#include "MacFiles/Classic.hh"

// MacFeatures
#include "MacFeatures/BlueBoxed.hh"
#include "MacFeatures/Features.hh"

// MacIO
#include "MacIO/FSMakeFSSpec_Sync.hh"
#include "MacIO/GetCatInfo_Sync.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Arcana / MD5
#include "MD5/MD5.hh"

// Pedestal
#include "Pedestal/WakeUp.hh"

// MacLamp
#include "FSSpec_from_stat.h"

// Genie
#include "Genie/code/executable_file.hh"
#include "Genie/code/prepare_executable.hh"
#include "Genie/FileSignature.hh"
#include "Genie/FS/file-tests.hh"
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/FSSpecForkUser.hh"
#include "Genie/FS/FSTreeCache.hh"
#include "Genie/FS/FSTree_RsrcFile.hh"
#include "Genie/FS/HFS/hashed_long_name.hh"
#include "Genie/FS/HFS/LongName.hh"
#include "Genie/FS/HFS/Rename.hh"
#include "Genie/FS/HFS/SetFileTimes.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/FS/sys/mac/errata.hh"
#include "Genie/FS/sys/mac/vol/list.hh"
#include "Genie/FS/ResFile_Dir.hh"
#include "Genie/FS/ResolvePathname.hh"
#include "Genie/FS/Root_Overlay.hh"
#include "Genie/FS/StatFile.hh"
#include "Genie/FS/Union.hh"
#include "Genie/FS/Users.hh"
#include "Genie/IO/MacDirectory.hh"
#include "Genie/IO/MacFile.hh"
#include "Genie/Kernel/native_syscalls.hh"
#include "Genie/Utilities/AsyncIO.hh"
#include "Genie/Utilities/CreateAlias.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	namespace path_descent_operators
	{
		
		static inline FSSpec operator/( const N::FSDirSpec& dir, const unsigned char* name )
		{
			return MacIO::FSMakeFSSpec< FNF_Returns >( dir, name );
		}
		
		static inline FSSpec operator/( const FSSpec& dir, const unsigned char* name )
		{
			return Dir_From_FSSpec( dir ) / name;
		}
		
		static inline FSSpec operator/( const N::FSDirSpec& dir, const plus::string& name )
		{
			return dir / N::Str63( name );
		}
		
		static inline FSSpec operator/( const FSSpec& dir, const plus::string& name )
		{
			return dir / N::Str63( name );
		}
		
	}
	
	using namespace path_descent_operators;
	
	
	static inline bool operator==( const FSSpec& a, const FSSpec& b )
	{
		const std::size_t length = sizeof (SInt16) + sizeof (UInt32) + 1 + a.name[0];
		
		return std::memcmp( &a, &b, length ) == 0;
	}
	
	
	static inline plus::string colons_from_slashes( const plus::string& mac_name )
	{
		return plus::replaced_string( mac_name, '/', ':' );
	}
	
	static inline plus::string slashes_from_colons( const plus::string& unix_name )
	{
		return plus::replaced_string( unix_name, ':', '/' );
	}
	
	
	static plus::string SlurpFile( const FSSpec& file )
	{
		plus::string result;
		
		n::owned< N::FSFileRefNum > input = N::FSpOpenDF( file, N::fsRdPerm );
		
		const std::size_t size = N::GetEOF( input );
		
		char* p = result.reset( size );
		
		const std::size_t bytes_read = N::FSRead( input, size, p, N::ThrowEOF_Always() );
		
		return result;
	}
	
	static void SpewFile( const FSSpec& file, const plus::string& contents )
	{
		n::owned< N::FSFileRefNum > output = FSpOpenDF( file, N::fsWrPerm );
		
		N::SetEOF( output, 0 );
		
		N::FSWrite( output, contents.size(), contents.data() );
	}
	
	static plus::string get_long_name( const FSSpec& item )
	{
		if ( item.name[0] == 31 )
		{
			try
			{
				plus::string comment = iota::convert_string< plus::string >( N::FSpDTGetComment( item ) );
				
				if ( comment.size() > 31 )
				{
					plus::string hashed = hashed_long_name( comment );
					
					ASSERT( hashed.size() == 31  &&  "Long filenames must hash to 31 chars" );
					
					if ( std::memcmp( hashed.data(), &item.name[1], 31 ) == 0 )
					{
						// Assume it's a Unix name.  FIXME:  Need better heuristics
						return comment;
					}
				}
			}
			catch ( ... )
			{
			}
		}
		
		return plus::string( item.name );
	}
	
	static plus::string GetUnixName( const FSSpec& item )
	{
		return plus::utf8_from_mac( colons_from_slashes( get_long_name( item ) ) );
	}
	
	static N::FSVolumeRefNum GetVRefNum( N::FSVolumeRefNum  vRefNum = N::FSVolumeRefNum() )
	{
		HParamBlockRec pb;
		
		pb.volumeParam.ioVRefNum  = vRefNum;
		pb.volumeParam.ioNamePtr  = NULL;
		pb.volumeParam.ioVolIndex = 0;  // use ioVRefNum only
		
		Mac::ThrowOSStatus( ::PBHGetVInfoSync( &pb ) );
		
		return N::FSVolumeRefNum( pb.volumeParam.ioVRefNum );
	}
	
	
	static const unsigned char* const_root_directory_name = "\p" "/";
	
	static N::FSDirSpec FindJDirectory()
	{
		CInfoPBRec cInfo = { 0 };
		
		N::Str63 name = const_root_directory_name;  // overkill
		
		// Try current directory first
		
		const bool exists = MacIO::GetCatInfo< FNF_Returns >( cInfo,
		                                                      N::FSVolumeRefNum(),
		                                                      N::FSDirID(),
		                                                      name );
		
		if ( exists )
		{
			cInfo.dirInfo.ioVRefNum = GetVRefNum( N::FSVolumeRefNum( cInfo.dirInfo.ioVRefNum ) );
		}
		else
		{
			// Then root, or bust
			N::FSDirSpec root = io::system_root< N::FSDirSpec >();
			
			MacIO::GetCatInfo< FNF_Throws >( cInfo,
			                                 root.vRefNum,
			                                 root.dirID,
			                                 name );
		}
		
		return Dir_From_CInfo( cInfo );
	}
	
	const Mac::FSDirSpec& root_DirSpec()
	{
		static N::FSDirSpec j = FindJDirectory();
		
		return j;
	}
	
	static N::FSDirSpec FindUsersDirectory()
	{
		N::FSDirSpec root = io::system_root< N::FSDirSpec >();
		
		FSSpec users = root / "\p" "Users";
		
		return Dir_From_FSSpec( users );
	}
	
	static const N::FSDirSpec& GetUsersDirectory()
	{
		static N::FSDirSpec users = FindUsersDirectory();
		
		return users;
	}
	
	
	static plus::string MakeName( const FSSpec& fileSpec )
	{
		if ( fileSpec.parID == fsRtParID )
		{
			return "mnt";
		}
		
		return GetUnixName( fileSpec );
	}
	
	
	class FSTree_HFS : public FSTree
	{
		private:
			FSSpec      itsFileSpec;
			CInfoPBRec  itsCInfo;
			bool        itIsOnServer;
		
		public:
			FSTree_HFS( const CInfoPBRec&    cInfo,
			            bool                 onServer,
			            const plus::string&  name,
			            const FSTree*        parent = NULL );
			
			FSTreePtr Parent() const;
			
			const FSSpec& GetFSSpec() const  { return itsFileSpec; }
			
			ino_t ParentInode() const  { return itsFileSpec.parID; }
			
			void Stat( struct ::stat& sb ) const;
			
			void ChangeMode( mode_t mode ) const;
			
			void SetTimes() const;
			
			void SetTimes( const struct timespec times[2] ) const;
			
			void Delete() const;
			
			void Rename( const FSTreePtr& destination ) const;
			
			ino_t Inode() const;
			
			void CopyFile( const FSTreePtr& destination ) const;
			
			off_t GetEOF() const;
			void  SetEOF( off_t length ) const;
			
			plus::string ReadLink() const;
			FSTreePtr ResolveLink() const;
			
			void SymLink( const plus::string& target ) const;
			
			IOPtr Open( OpenFlags flags, mode_t mode ) const;
			IOPtr Open( OpenFlags flags              ) const;
			
			shared_exec_handle GetExecutable() const;
			
			IOPtr OpenDirectory() const;
			
			void CreateDirectory( mode_t mode ) const;
			
			FSTreePtr Lookup_Child( const plus::string& name, const FSTree* parent ) const;
			
			void IterateIntoCache( FSTreeCache& cache ) const;
			
			FSTreePtr ResolvePath( const char*& begin, const char* end ) const;
		
		private:
			void CreateFile() const;
			
			void FinishCreation() const;
	};
	
	static FSSpec FSMakeFSSpec( const CInfoPBRec& cInfo )
	{
		const HFileInfo& hFileInfo = cInfo.hFileInfo;
		
		const bool exists = hFileInfo.ioResult == noErr;
		
		const FSVolumeRefNum vRefNum = hFileInfo.ioVRefNum;
		
		const UInt32 parID = exists ? hFileInfo.ioFlParID
		                            : hFileInfo.ioDirID;
		
		FSSpec result = { vRefNum, parID };
		
		memcpy( result.name, hFileInfo.ioNamePtr, 1 + hFileInfo.ioNamePtr[0] );
		
		return result;
	}
	
	static void hfs_symlink( const FSTree*        node,
	                         const plus::string&  target )
	{
		const FSTree_HFS* file = static_cast< const FSTree_HFS* >( node );
		
		file->SymLink( target );
	}
	
	static node_method_set hfs_methods =
	{
		NULL,
		&hfs_symlink
	};
	
	FSTree_HFS::FSTree_HFS( const CInfoPBRec&    cInfo,
	                        bool                 onServer,
	                        const plus::string&  name,
	                        const FSTree*        parent )
	:
		FSTree( parent ? parent->Self() : null_FSTreePtr,
		        name,
		        GetItemMode( cInfo.hFileInfo ),
		        &hfs_methods ),
		itsFileSpec     ( FSMakeFSSpec( cInfo ) ),
		itsCInfo        ( cInfo                 ),
		itIsOnServer    ( onServer              )
	{
		// we override Parent()
		
		ASSERT( !name.empty() );
		
		itsCInfo.hFileInfo.ioNamePtr = itsFileSpec.name;
	}
	
	
	static void FSpFileCopy( const FSSpec&         source,
	                         const FSSpec&         destDir,
	                         const unsigned char*  copyName       = NULL,
	                         void*                 copyBufferPtr  = NULL,
	                         long                  copyBufferSize = 0,
	                         bool                  preflight      = true )
	{
		NUCLEUS_REQUIRE_ERRORS( Nitrogen::FileManager );
		
		Mac::ThrowOSStatus( ::FSpFileCopy( &source,
		                                   &destDir,
		                                   copyName,
		                                   copyBufferPtr,
		                                   copyBufferSize,
		                                   preflight ) );
	}
	
	void FSTree_HFS::CopyFile( const FSTreePtr& destination ) const
	{
		const FSSpec& srcFile = itsFileSpec;
		
		const FSSpec destFile = GetFSSpecFromFSTree( destination );
		
		// Do not resolve links
		
		N::FSDirSpec destDir = io::get_preceding_directory( destFile );
		
		const bool renaming = !std::equal( srcFile.name,
		                                   srcFile.name + 1 + srcFile.name[0],
		                                   destFile.name );
		
		ConstStr255Param name = renaming ? destFile.name : NULL;
		
		// FIXME:  This logic should be worked into the file copy routine
		// Maybe use ExchangeFiles() for safety?
		
		if ( io::file_exists( destFile ) )
		{
			io::delete_file( destFile );
		}
		
		FSpFileCopy( srcFile, destDir / "\p", name );
	}
	
	
	FSTreePtr FSTreeFromFSSpec( const FSSpec& item, bool onServer )
	{
		CInfoPBRec cInfo;
		
		FSpGetCatInfo< FNF_Returns >( cInfo,
		                              onServer,
		                              item );
		
		const plus::string name = MakeName( item );
		
		return new FSTree_HFS( cInfo, onServer, name );
	}
	
	FSTreePtr FSTreeFromFSDirSpec( const N::FSDirSpec& dir, bool onServer )
	{
		N::Str31 mac_name = "\p";
		
		CInfoPBRec cInfo;
		
		FSpGetCatInfo< FNF_Throws >( cInfo,
		                             onServer,
		                             dir.vRefNum,
		                             dir.dirID,
		                             mac_name,
		                             0 );
		
		const FSSpec fsspec = FSMakeFSSpec( cInfo );
		
		const plus::string name = MakeName( fsspec );
		
		return new FSTree_HFS( cInfo, onServer, name );
	}
	
	FSTreePtr New_FSTree_Users( const FSTreePtr&     parent,
	                            const plus::string&  name,
	                            const void*          args )
	{
		return FSTreeFromFSDirSpec( GetUsersDirectory(), false );
	}
	
	
	static const FSTreePtr& MakeFSRoot()
	{
		const Mac::FSDirSpec& j_dir = root_DirSpec();
		
		FSTree_Union* u = NULL;
		
		static FSTreePtr result = u = new FSTree_Union( null_FSTreePtr, plus::string::null );
		
		if ( u != NULL )
		{
			FSTreePtr top    = Premapped_Factory( null_FSTreePtr, plus::string::null, Root_Overlay_Mappings );
			FSTreePtr bottom = FSTreeFromFSDirSpec( j_dir, false );
			
			u->SetTop   ( top    );
			u->SetBottom( bottom );
		}
		
		return result;
	}
	
	const FSTreePtr& FSRoot()
	{
		static const FSTreePtr& root = MakeFSRoot();
		
		return root;
	}
	
	
	FSTreePtr FSTree_HFS::Parent() const
	{
		if ( itsFileSpec.parID == fsRtParID )
		{
			return Get_sys_mac_vol_N( N::FSVolumeRefNum( itsFileSpec.vRefNum ) );
		}
		
		const Mac::FSDirSpec& root = root_DirSpec();
		
		if ( itsFileSpec.vRefNum == root.vRefNum  &&  itsFileSpec.parID == root.dirID )
		{
			return FSRoot();
		}
		
		try
		{
			if ( itsFileSpec == GetUsersDirectory() / "\p" )
			{
				return FSRoot();
			}
		}
		catch ( ... )
		{
		}
		
		return FSTreePtr( FSTreeFromFSDirSpec( io::get_preceding_directory( itsFileSpec ),
		                                       itIsOnServer ) );
	}
	
	ino_t FSTree_HFS::Inode() const
	{
		struct ::stat sb;
		
		Stat( sb );
		
		return sb.st_ino;
	}
	
	void FSTree_HFS::Stat( struct ::stat& sb ) const
	{
		Stat_HFS( itIsOnServer, &sb, itsCInfo, itsFileSpec.name, false );
	}
	
	void FSTree_HFS::ChangeMode( mode_t mode ) const
	{
		ChangeFileMode( GetFSSpec(), mode );
	}
	
	void FSTree_HFS::SetTimes() const
	{
		SetFileTimes( N::FSVolumeRefNum( itsFileSpec.vRefNum ),
		              N::FSDirID       ( itsFileSpec.parID   ),
		              itsFileSpec.name );
	}
	
	void FSTree_HFS::SetTimes( const struct timespec times[2] ) const
	{
		SetFileTimes( N::FSVolumeRefNum( itsFileSpec.vRefNum ),
		              N::FSDirID       ( itsFileSpec.parID   ),
		              itsFileSpec.name,
		              times );
	}
	
	static void Delete_HFS( const FSSpec& fileSpec )
	{
		// returns fnfErr for directories
		OSErr unlockErr = ::FSpRstFLock( &fileSpec );
		
		OSErr deleteErr = ::FSpDelete( &fileSpec );
		
		if ( MacFeatures::Is_Running_OSXNative()  &&  unlockErr == noErr  &&  deleteErr == fBsyErr )
		{
			// If we're on OS X and the file was busy, try the native unlink().
			
			const FSRef fsRef = N::FSpMakeFSRef( fileSpec );
			
			const nucleus::string path = N::FSRefMakePath( fsRef );
			
			p7::throw_posix_result( native_unlink( path.c_str() ) );
			
			return;
		}
		
		// Unfortunately, fBsyErr can mean different things.
		// Here we assume it means a directory is not empty.
		
		if ( unlockErr == fnfErr  &&  deleteErr == fBsyErr )
		{
			p7::throw_errno( ENOTEMPTY );
		}
		
		Mac::ThrowOSStatus( deleteErr );
	}
	
	void FSTree_HFS::Delete() const
	{
		Delete_HFS( itsFileSpec );
	}
	
	
	FSSpec GetFSSpecFromFSTree( const FSTreePtr& file )
	{
		struct ::stat stat_buffer = { 0 };
		
		try
		{
			file->Stat( stat_buffer );
		}
		catch ( const p7::errno_t& err )
		{
			if ( err != ENOENT )
			{
				throw;
			}
		}
		
		FSSpec spec;
		
		p7::throw_posix_result( FSSpec_from_stat( stat_buffer, spec ) );
		
		return spec;
	}
	
	void FSTree_HFS::Rename( const FSTreePtr& destFile ) const
	{
		Rename_HFS( itsFileSpec, destFile );
	}
	
	
	off_t FSTree_HFS::GetEOF() const
	{
		Mac::ThrowOSStatus( itsCInfo.hFileInfo.ioResult );
		
		return itsCInfo.hFileInfo.ioFlLgLen;
	}
	
	void FSTree_HFS::SetEOF( off_t length ) const
	{
		N::SetEOF( N::FSpOpenDF( GetFSSpec(), N::fsWrPerm ), length );
	}
	
	plus::string FSTree_HFS::ReadLink() const
	{
		if ( !IsLink() )
		{
			p7::throw_errno( EINVAL );
		}
		
		const plus::string target = SlurpFile( itsFileSpec );
		
		if ( !target.empty() )
		{
			return target;
		}
		
		return ResolveLink()->Pathname();
	}
	
	FSTreePtr FSTree_HFS::ResolveLink() const
	{
		if ( !IsLink() )
		{
			return Self();
		}
		
		const HFileInfo& hFileInfo = itsCInfo.hFileInfo;
		
		const bool exists = hFileInfo.ioResult == noErr;
		
		if ( const bool is_dir = hFileInfo.ioFlAttrib & kioFlAttribDirMask )
		{
			// Symlink to root directory
			return FSRoot();
		}
		else
		{
			const FInfo& fInfo = hFileInfo.ioFlFndrInfo;
			
			const bool is_alias = fInfo.fdFlags & kIsAlias;
			
			const plus::string target = SlurpFile( itsFileSpec );
			
			if ( !target.empty() )
			{
				return ResolvePathname( target, Parent() );
			}
			else if ( is_alias )
			{
				FSSpec target = N::ResolveAliasFile( GetFSSpec(), false );
				
				return FSTreeFromFSSpec( target, FileIsOnServer( target ) );
			}
			else
			{
				// empty 'slnk' file
			//	throw p7::errno_t( EIO );
			}
		}
		
		// Appease Metrowerks C++, which thinks we don't return a value
		throw p7::errno_t( EIO );
	}
	
	static void create_native_symlink( const FSSpec& link_spec, const char* target_path )
	{
		plus::string utf8_link_name = plus::utf8_from_mac( link_spec.name );
		
		FSSpec parent_spec = N::FSMakeFSSpec( io::get_preceding_directory( link_spec ) );
		
		FSRef parent_ref = N::FSpMakeFSRef( parent_spec );
		
		nucleus::mutable_string path = N::FSRefMakePath( parent_ref );
		
		path += '/';
		
		path.append( utf8_link_name.data(), utf8_link_name.size() );
		
		p7::throw_posix_result( native_symlink( target_path,
		                                        path.c_str() ) );
	}
	
	static void CreateSymLink( const FSTreePtr& linkFile, const plus::string& targetPath )
	{
		FSSpec linkSpec = GetFSSpecFromFSTree( linkFile );
		
		if ( MacFeatures::Is_Running_OSXNative() )
		{
			create_native_symlink( linkSpec, targetPath.c_str() );
			
			return;
		}
		
		N::FSDirSpec linkParent = io::get_preceding_directory( linkSpec );
		
		try
		{
			// Target path is resolved relative to the location of the link file
			// This throws if a nonterminal path component is missing
			FSTreePtr target = ResolvePathname( targetPath, FSTreeFromFSDirSpec( linkParent, FileIsOnServer( linkSpec ) ) );
			
			// Do not resolve links -- if the target of this link is another symlink, so be it
			
			FSSpec targetSpec = GetFSSpecFromFSTree( target );
			
			CreateAlias( linkSpec, targetSpec );
		}
		catch ( const Mac::OSStatus& err )
		{
			// Non-aliases get creator and type for OS X symlinks
			N::FSpCreate( linkSpec, Mac::kSymLinkCreator, Mac::kSymLinkFileType );
		}
		
		SpewFile( linkSpec, targetPath );
	}
	
	void FSTree_HFS::SymLink( const plus::string& target ) const
	{
		CreateSymLink( Self(), target );
		
		FinishCreation();
	}
	
	void FSTree_HFS::CreateFile() const
	{
		const plus::string& name = Name();
		
		N::FileSignature sig = PickFileSignatureForName( name.data(), name.size() );
		
		N::FSpCreate( itsFileSpec, sig );
		
		FinishCreation();
	}
	
	IOPtr FSTree_HFS::Open( OpenFlags flags, mode_t mode ) const
	{
		bool creating  = flags & O_CREAT;
		bool excluding = flags & O_EXCL;
		
		bool created = false;
		
		if ( creating )
		{
			const bool exists = itsCInfo.hFileInfo.ioResult == noErr;
			
			if ( !exists )
			{
				CreateFile();
				
				created = true;
			}
			else if ( excluding )
			{
				p7::throw_errno( EEXIST );
			}
			else
			{
				const plus::string name_MacRoman = plus::mac_from_utf8( Name() );
				
				if ( name_MacRoman.size() <= 31 )
				{
					// Long names are case-sensitive due to hashing
					
					const plus::string name = slashes_from_colons( name_MacRoman );
					
					const bool equal = std::equal( name.begin(),
					                               name.end(),
					                               (const char*) (itsFileSpec.name + 1) );
					
					if ( !equal )
					{
						N::FSpRename( itsFileSpec, name );
					}
				}
			}
		}
		
		IOPtr opened = Open( flags );
		
		if ( created )
		{
			ChangeMode( mode );
		}
		
		return opened;
	}
	
	IOPtr FSTree_HFS::Open( OpenFlags flags ) const
	{
		flags |= itIsOnServer ? O_MAC_ASYNC : 0;
		
		return OpenMacFileHandle( GetFSSpec(),
		                          flags,
		                          itIsOnServer ? &Genie::FSpOpenDF : N::FSpOpenDF,
		                          &New_DataForkHandle );
	}
	
	shared_exec_handle FSTree_HFS::GetExecutable() const
	{
		execution_unit unit = load_executable_file( GetFSSpec() );
		
		return prepare_executable( unit );
	}
	
	IOPtr FSTree_HFS::OpenDirectory() const
	{
		if ( !IsDirectory() )
		{
			p7::throw_errno( Exists() ? ENOTDIR : ENOENT );
		}
		
		const N::FSDirSpec dir = Dir_From_CInfo( itsCInfo );
		
		return new MacDirHandle( dir, itIsOnServer );
	}
	
	void FSTree_HFS::CreateDirectory( mode_t /*mode*/ ) const
	{
		N::FSpDirCreate( itsFileSpec );
		
		FinishCreation();
	}
	
	
	static FSTreePtr FSTreePtr_From_Lookup( const N::FSDirSpec&  dir,
	                                        bool                 onServer,
	                                        const plus::string&  name,
	                                        const FSTree*        parent )
	{
		N::Str31 macName = hashed_long_name( slashes_from_colons( plus::mac_from_utf8( name ) ) );
		
		CInfoPBRec cInfo;
		
		FSpGetCatInfo< FNF_Returns >( cInfo, onServer, dir.vRefNum, dir.dirID, macName, 0 );
		
		return new FSTree_HFS( cInfo, onServer, name, parent );
	}
	
	FSTreePtr FSTree_HFS::Lookup_Child( const plus::string& name, const FSTree* parent ) const
	{
		if ( name == "rsrc"  &&  IsFile() )
		{
			return GetRsrcForkFSTree( itsFileSpec, itIsOnServer );
		}
		
		if ( name == "r"  &&  IsFile() )
		{
			return Get_ResFileDir_FSTree( Self(), "r", itsFileSpec );
		}
		
		Mac::ThrowOSStatus( itsCInfo.dirInfo.ioResult );
		
		N::FSDirSpec dir = Dir_From_CInfo( itsCInfo );
		
		return FSTreePtr_From_Lookup( dir, itIsOnServer, name, parent );
	}
	
	
	const UInt16 kMaxItems = 4;
	
	struct NameAndID
	{
		N::Str31    name;
		N::FSDirID  id;
	};
	
#ifdef __MACOS__
	
	struct IterateIntoCache_CInfoPBRec : CInfoPBRec
	{
		NameAndID      items[ kMaxItems ];
		UInt16         n_items;
		volatile bool  done;
	};
	
	namespace
	{
		
		pascal void IterateIntoCache_Completion( ParamBlockRec* pb )
		{
			IterateIntoCache_CInfoPBRec& cInfo = *(IterateIntoCache_CInfoPBRec*) pb;
			
			if ( cInfo.dirInfo.ioResult != noErr )
			{
				goto done;
			}
			
			cInfo.items[ cInfo.n_items ].id = N::FSDirID( cInfo.dirInfo.ioDrDirID );
			
			++cInfo.n_items;
			
			if ( cInfo.n_items == kMaxItems )
			{
				goto done;
			}
			
			cInfo.dirInfo.ioNamePtr = cInfo.items[ cInfo.n_items ].name;
			
			cInfo.dirInfo.ioNamePtr[ 0 ] = '\0';
			
			cInfo.dirInfo.ioDrDirID = cInfo.dirInfo.ioDrParID;
			
			++cInfo.dirInfo.ioFDirIndex;
			
			OSErr err = ::PBGetCatInfoAsync( &cInfo );
			
			if ( cInfo.dirInfo.ioResult >= 0 )
			{
				// Successfully queued
				return;
			}
			
			// We don't know if this was queued or not, so set done below
			
		done:
			
			cInfo.done = true;
			
			Ped::WakeUp();
		}
		
	}
	
	static void IterateFilesIntoCache( IterateIntoCache_CInfoPBRec&  pb,
	                                   FSTreeCache&                  cache )
	{
		FSSpec item = { pb.dirInfo.ioVRefNum, pb.dirInfo.ioDrDirID };
		
		N::FSDirID dirID = N::FSDirID( pb.dirInfo.ioDrDirID );
		
		const bool async = !TARGET_CPU_68K && FileIsOnServer( item ) && !MacFeatures::Is_BlueBoxed();
		
		if ( async )
		{
			pb.dirInfo.ioCompletion = N::StaticUPP< N::IOCompletionUPP, IterateIntoCache_Completion >();
		}
		
		UInt16 n_items = 0;
		
		while ( true )
		{
			const UInt16 i = n_items + 1;  // one-based
			
			pb.dirInfo.ioNamePtr = pb.items[ 0 ].name;
			pb.dirInfo.ioDrDirID = dirID;
			
			pb.dirInfo.ioFDirIndex = i;
			
			pb.done = false;
			
			pb.n_items = 0;
			
			pb.items[ 0 ].name[ 0 ] = '\0';
			
			if ( async )
			{
				N::PBGetCatInfoAsync( pb, N::FNF_Returns() );
				
				while ( !pb.done )
				{
					AsyncYield();
				}
			}
			else if ( const bool exists = N::PBGetCatInfoSync( pb, N::FNF_Returns() ) )
			{
				pb.items[ 0 ].id = N::FSDirID( pb.dirInfo.ioDrDirID );
				
				++pb.n_items;
			}
			
			n_items += pb.n_items;
			
			for ( UInt16 j = 0;  j != pb.n_items;  ++j )
			{
				const ino_t inode = pb.items[ j ].id;  // file or dir ID for inode
				
				N::CopyToPascalString( pb.items[ j ].name, item.name, sizeof item.name - 1 );
				
				const FSNode node( inode, GetUnixName( item ) );
				
				cache.push_back( node );
			}
			
			if ( pb.dirInfo.ioResult == fnfErr )
			{
				return;
			}
			
			Mac::ThrowOSStatus( pb.dirInfo.ioResult );
		}
	}
	
#endif
	
	void FSTree_HFS::IterateIntoCache( FSTreeCache& cache ) const
	{
		Mac::ThrowOSStatus( itsCInfo.hFileInfo.ioResult );
		
	#ifdef __MACOS__
		
		IterateIntoCache_CInfoPBRec cInfo;
		
		static_cast< CInfoPBRec& >( cInfo ) = itsCInfo;
		
		IterateFilesIntoCache( cInfo, cache );
		
	#endif
	}
	
#ifdef __MACOS__
	
	struct ResolvePath_CInfoPBRec : CInfoPBRec
	{
		N::Str63       name;
		const char*    begin;
		const char*    end;
		volatile bool  done;
	};
	
	static inline bool char_is_special( char c )
	{
		return c == ':'  ||  c & 0x80;
	}
	
	static bool name_is_special( const char* begin, const char* end )
	{
		const unsigned name_length = end - begin;
		
		// This is the UTF-8-encoded length, which will be longer than the
		// MacRoman length in the presence of non-ASCII characters, but those
		// are special anyway.
		
		if ( name_length > 31 )
		{
			return true;
		}
		else if ( const bool dot = begin[0] == '.' )
		{
			const bool two_dots = begin[1] == '.';
			
			if ( const bool thats_it = 1 + two_dots == name_length )
			{
				return true;
			}
		}
		else if ( std::find_if( begin, end, std::ptr_fun( char_is_special ) ) != end )
		{
			return true;
		}
		
		return false;
	}
	
	namespace
	{
		
		pascal void ResolvePath_Completion( ParamBlockRec* pb )
		{
			ResolvePath_CInfoPBRec& cInfo = *(ResolvePath_CInfoPBRec*) pb;
			
			if ( cInfo.dirInfo.ioResult < 0 )
			{
				goto done;
			}
			
			const char* begin = cInfo.begin;
			
			while ( begin < cInfo.end  &&  begin[0] == '/' )
			{
				++begin;
			}
			
			if ( begin == cInfo.end )
			{
				goto done;
			}
			
			const bool is_dir = cInfo.hFileInfo.ioFlAttrib & kioFlAttribDirMask;
			
			if ( !is_dir )
			{
				goto done;
			}
			
			const char* slash = std::find( begin, cInfo.end, '/' );
			
			if ( name_is_special( begin, slash ) )
			{
				goto done;
			}
			
			const unsigned name_length = slash - begin;
			
			cInfo.name[ 0 ] = name_length;
			
			std::copy( begin, slash, &cInfo.name[1] );
			
			cInfo.begin = slash;
			
			OSErr err = ::PBGetCatInfoAsync( &cInfo );
			
			if ( cInfo.dirInfo.ioResult >= 0 )
			{
				// Successfully queued
				return;
			}
			
			// We don't know if this was queued or not, so set done below
			
		done:
			
			cInfo.done = true;
			
			Ped::WakeUp();
		}
		
	}
	
	static FSTreePtr ResolvePath_HFS( const FSSpec& dirFileSpec, const char*& begin, const char* end )
	{
		ResolvePath_CInfoPBRec cInfo;
		
		DirInfo& dirInfo = cInfo.dirInfo;
		
		dirInfo.ioCompletion = N::StaticUPP< N::IOCompletionUPP, ResolvePath_Completion >();
		
		dirInfo.ioNamePtr   = cInfo.name;
		dirInfo.ioVRefNum   = dirFileSpec.vRefNum;
		dirInfo.ioDrDirID   = dirFileSpec.parID;
		dirInfo.ioFDirIndex = 0;
		
		cInfo.name = dirFileSpec.name;
		
		cInfo.begin = begin;
		cInfo.end   = end;
		cInfo.done  = false;
		
		N::PBGetCatInfoAsync( cInfo, N::FNF_Returns() );
		
		while ( !cInfo.done )
		{
			AsyncYield();
		}
		
		begin = cInfo.begin;
		
		ASSERT( dirInfo.ioResult <= 0 );
		
		const bool nonexistent = dirInfo.ioResult == fnfErr  && begin == end;
		
		if ( !nonexistent )
		{
			Mac::ThrowOSStatus( dirInfo.ioResult );
		}
		
		const SInt32 dirID = nonexistent ? dirInfo.ioDrDirID : dirInfo.ioDrParID;
		
		FSSpec result = { dirInfo.ioVRefNum, dirID };
		
		N::CopyToPascalString( cInfo.name, result.name, sizeof result.name - 1 );
		
		const plus::string name = MakeName( result );
		
		// This code path is only used on servers.
		
		return new FSTree_HFS( cInfo, true, name );
	}
	
#endif
	
	FSTreePtr FSTree_HFS::ResolvePath( const char*& begin, const char* end ) const
	{
		ASSERT( begin < end );
		
	#ifdef __MACOS__
		if (     TARGET_CPU_68K
		     ||  !itIsOnServer
		     ||  !IsDirectory()
		     ||  name_is_special( begin, std::find( begin, end, '/' ) )
		     ||  MacFeatures::Is_BlueBoxed() )
	#endif
		{
			// Special handling required for
			// * dot, dotdot, colons, and long names
			// * running in Classic, which has broken PBGetCatInfoAsync()
			
			return FSTree::ResolvePath( begin, end );
		}
		
	#ifdef __MACOS__
		
		return ResolvePath_HFS( itsFileSpec, begin, end );
		
	#endif
	}
	
	void FSTree_HFS::FinishCreation() const
	{
		SetLongName( itsFileSpec, slashes_from_colons( plus::mac_from_utf8( Name() ) ) );
	}
	
}

