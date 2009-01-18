/*	================
 *	FSTree_FSSpec.cc
 *	================
 */

#include "Genie/FileSystem/FSTree_FSSpec.hh"

// Standard C++
#include <algorithm>
#include <functional>
#include <map>
#include <vector>

// POSIX
#include "fcntl.h"
#include "utime.h"
#include "sys/stat.h"

// MoreFiles
#include "FileCopy.h"
#include "MoreFilesExtras.h"

// Nucleus
#include "Nucleus/NAssert.h"

// Nitrogen
#include "Nitrogen/Aliases.h"
#include "Nitrogen/Files.h"
#include "Nitrogen/Resources.h"

// Io: MacFiles
#include "MacFiles.hh"

// POSeven
#include "POSeven/Errno.hh"

// TimeOff
#include "TimeOff.hh"

// Arcana / MD5
#include "MD5.hh"

// Kerosene
#include "MacFilenameFromUnixFilename.hh"

// Genie
#include "Genie/FileSignature.hh"
#include "Genie/FileSystem/FSSpecForkUser.hh"
#include "Genie/FileSystem/FSTree_Dev.hh"
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/FileSystem/FSTree_Proc.hh"
#include "Genie/FileSystem/FSTree_RsrcFile.hh"
#include "Genie/FileSystem/FSTree_new.hh"
#include "Genie/FileSystem/FSTree_sys.hh"
#include "Genie/FileSystem/FSTree_sys_mac_vol.hh"
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/FileSystem/StatFile.hh"
#include "Genie/IO/MacFile.hh"
#include "Genie/Utilities/AsyncIO.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	namespace K = Kerosene;
	
	
	using namespace io::path_descent_operators;
	
	
	inline bool operator==( const FSSpec& a, const FSSpec& b )
	{
		const std::size_t length = sizeof (SInt16) + sizeof (UInt32) + 1 + a.name[0];
		
		return std::memcmp( &a, &b, length ) == 0;
	}
	
	
	static std::string MacFromUnixName( const std::string& name )
	{
		//ASSERT( name != "."  );
		//ASSERT( name != ".." );
		
		std::string result;
		
		result.resize( name.size() );
		
		std::replace_copy( name.begin(),
		                   name.end(),
		                   result.begin(),
		                   ':',
		                   '/' );
		
		return result;
	}
	
	static std::string UnixFromMacName( const std::string& name )
	{
		std::string result;
		
		result.resize( name.size() );
		
		std::replace_copy( name.begin(),
		                   name.end(),
		                   result.begin(),
		                   '/',
		                   ':' );
		
		return result;
	}
	
	
	static FSSpec FSSpecForLongUnixName( const N::FSDirSpec& parent, const std::string& unixName )
	{
		FSSpec result = parent / K::MacFilenameFromUnixFilename( unixName );
		
		return result;
	}
	
	static FSSpec FSSpecForLongUnixName( const N::FSDirSpec&  parent,
	                                     const std::string&   unixName,
	                                     bool                 forCreation )
	{
		FSSpec result = FSSpecForLongUnixName( parent, unixName );
		
		if ( io::item_exists( result ) == forCreation )
		{
			p7::throw_errno( forCreation ? ENAMETOOLONG : ENOENT );
		}
		
		return result;
	}
	
	inline FSSpec NewFSSpecForLongUnixName( const N::FSDirSpec& parent, const std::string& unixName )
	{
		return FSSpecForLongUnixName( parent, unixName, true );
	}
	
	inline FSSpec OldFSSpecForLongUnixName( const N::FSDirSpec& parent, const std::string& unixName )
	{
		return FSSpecForLongUnixName( parent, unixName, false );
	}
	
	static bool ItemWithLongNameExists( const N::FSDirSpec& parent, const std::string& unixName )
	{
		FSSpec item = FSSpecForLongUnixName( parent, unixName );
		
		return io::item_exists( item );
	}
	
	static void CreateFileWithLongName( const N::FSDirSpec& parent, const std::string& unixName )
	{
		N::FileSignature sig = PickFileSignatureForName( unixName );
		
		FSSpec file = NewFSSpecForLongUnixName( parent, unixName );
		
		N::FSpCreate( file, sig );
		
		N::FSpDTSetComment( file, unixName );
	}
	
	static void CreateDirectoryWithLongName( const N::FSDirSpec& parent, const std::string& unixName )
	{
		FSSpec dir = NewFSSpecForLongUnixName( parent, unixName );
		
		N::FSpDirCreate( dir );
		
		N::FSpDTSetComment( dir, unixName );
	}
	
	
	static std::string GetUnixName( const FSSpec& item )
	{
		if ( item.name[0] == 31 )
		{
			try
			{
				std::string comment = N::FSpDTGetComment( item );
				
				if ( comment.size() > 31 )
				{
					// throws ENOENT if the encoded name doesn't exist
					FSSpec hashed = OldFSSpecForLongUnixName( io::get_preceding_directory( item ), comment );
					
					if ( hashed == item )
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
		
		return UnixFromMacName( io::get_filename_string( item ) );
	}
	
	
	static N::FSVolumeRefNum DetermineVRefNum( ConstStr255Param   name,
	                                           N::FSVolumeRefNum  vRefNum = N::FSVolumeRefNum() )
	{
		::FSVolumeRefNum actualVRefNum;
		N::ThrowOSStatus( ::DetermineVRefNum( name, vRefNum, &actualVRefNum ) );
		
		return N::FSVolumeRefNum( actualVRefNum );
	}
	
	static N::FSVolumeRefNum DetermineVRefNum( const std::string&  name,
	                                           N::FSVolumeRefNum   vRefNum = N::FSVolumeRefNum() )
	{
		return DetermineVRefNum( N::Str27( name ), vRefNum );
	}
	
	
	static const char* const_j_directory_name = "j";
	
	static FSSpec FindJDirectory()
	{
		// Try current directory first
		FSSpec j =  N::FSDirSpec() / const_j_directory_name;
		
		if ( !io::directory_exists( j ) )
		{
			// Then root, or bust
			j = io::system_root< N::FSDirSpec >() / const_j_directory_name;
			
			(void) N::FSDirSpec( j );  // throws if not a dir
		}
		
		return j;
	}
	
	static const FSSpec& GetJDirectory()
	{
		static FSSpec j = FindJDirectory();
		
		return j;
	}
	
	inline bool IsRootDirectory( const FSSpec& fileSpec )
	{
		return fileSpec == GetJDirectory();
	}
	
	static FSSpec FindUsersDirectory()
	{
		N::FSDirSpec root = io::system_root< N::FSDirSpec >();
		
		FSSpec users = root / "Users";
		
		(void) N::FSDirSpec( users );  // throws if not a dir
		
		return users;
	}
	
	static const FSSpec& GetUsersDirectory()
	{
		static FSSpec users = FindUsersDirectory();
		
		return users;
	}
	
	
	struct Volume_KeyName_Traits
	{
		typedef N::FSVolumeRefNum Key;
		
		static FSSpec FSSpecFromKey( const Key& key )
		{
			return N::FSMakeFSSpec( key, N::fsRtDirID, "\p" );
		}
		
		static std::string NameFromKey( const Key& key )
		{
			return UnixFromMacName( io::get_filename_string( FSSpecFromKey( key ) ) );
		}
		
		static Key KeyFromName( const std::string& name )
		{
			Key key = DetermineVRefNum( MacFromUnixName( name ) + ":" );
			
			return key;
		}
	};
	
	
	static std::string MakeName( const FSSpec& fileSpec )
	{
		if ( fileSpec.parID == fsRtParID )
		{
			return "mnt";
		}
		
		if ( IsRootDirectory( fileSpec ) )
		{
			return "";
		}
		
		return GetUnixName( fileSpec );
	}
	
	
	class FSTree_HFS : public FSTree_Mappable
	{
		private:
			FSSpec itsFileSpec;
		
		public:
			FSTree_HFS( const FSSpec&       file,
			            const std::string&  name = std::string() );
			
			bool Exists() const;
			bool IsFile() const;
			bool IsDirectory() const;
			bool IsLink() const;
			
			FSTreePtr Parent() const;
			
			FSSpec GetFSSpec( bool forCreation = false ) const;
			
			ino_t ParentInode() const;
			
			void Stat( struct ::stat& sb ) const;
			
			void ChangeMode( mode_t mode ) const;
			
			void SetTimes() const;
			
			void SetTimes( const struct timeval* access,
			               const struct timeval* mod,
			               const struct timeval* backup,
			               const struct timeval* creat ) const;
			
			void Delete() const;
			
			void Rename( const FSTreePtr& destination ) const;
			
			ino_t Inode() const;
			
			void CopyFile( const FSTreePtr& destination ) const;
			
			off_t GetEOF() const;
			void  SetEOF( off_t length ) const;
			
			std::string ReadLink() const;
			FSTreePtr ResolveLink() const;
			
			void SymLink( const std::string& target ) const;
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags, mode_t mode ) const;
			boost::shared_ptr< IOHandle > Open( OpenFlags flags              ) const;
			
			MainEntry GetMainEntry() const;
			
			void CreateDirectory( mode_t mode ) const;
			
			FSTreePtr Lookup_Regular( const std::string& name ) const;
			
			void IterateIntoCache( FSTreeCache& cache ) const;
		
		private:
			virtual void CreateFile() const;
	};
	
	FSTree_HFS::FSTree_HFS( const FSSpec&       file,
	                        const std::string&  name )
	:
		FSTree_Mappable( FSTreePtr(), name.empty() ? MakeName( file )
		                                           : name ),
		itsFileSpec    ( file                             )
	{
		// we override Parent()
	}
	
	static void FSpFileCopy( const FSSpec&         source,
	                         const FSSpec&         destDir,
	                         const unsigned char*  copyName       = NULL,
	                         void*                 copyBufferPtr  = NULL,
	                         long                  copyBufferSize = 0,
	                         bool                  preflight      = true )
	{
		NUCLEUS_REQUIRE_ERRORS( Nitrogen::FileManager );
		
		N::ThrowOSStatus( ::FSpFileCopy( &source,
		                                 &destDir,
		                                 copyName,
		                                 copyBufferPtr,
		                                 copyBufferSize,
		                                 preflight ) );
	}
	
	void FSTree_HFS::CopyFile( const FSTreePtr& destination ) const
	{
		const FSSpec srcFile = GetFSSpec( false );
		
		const FSSpec destFile = destination->GetFSSpec( true );
		
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
		
		FSpFileCopy( srcFile, destDir / "", name );
	}
	
	
	class FSTree_FSSpec : public FSTree_HFS
	{
		public:
			FSTree_FSSpec( const FSSpec& file ) : FSTree_HFS( file )
			{
			}
			
			FSTree_FSSpec( const N::FSDirSpec& dir ) : FSTree_HFS ( NN::Convert< FSSpec >( dir ) )
			{
			}
	};
	
	class FSTree_LongName : public FSTree_HFS
	{
		private:
			N::FSDirSpec  itsParent;
		
		public:
			FSTree_LongName( const N::FSDirSpec&  parent,
			                 const std::string&   name )
			:
				FSTree_HFS( FSSpecForLongUnixName( parent, name ), name ),
				itsParent ( parent )
			{
			}
			
			bool Exists() const;
			bool IsFile() const;
			bool IsDirectory() const;
			
			FSTreePtr Parent() const;
			
			FSSpec GetFSSpec( bool forCreation = false ) const;
			
			void CreateFile() const;
			
			void CreateDirectory( mode_t mode ) const;
	};
	
	
	class FSTree_Volumes_Link;
	
	
	struct Volumes_Details : public Volume_KeyName_Traits
	{
		typedef N::Volume_Container Sequence;
		
		static FSTreePtr Parent()  { return FSRoot(); }
		
		static Sequence ItemSequence()  { return N::Volumes(); }
		
		static Key KeyFromValue( const Sequence::value_type& value )  { return value; }
		
		static bool KeyIsValid( const Key& key )
		{
			return true;  // DetermineVRefNum() only returns valid keys
		}
		
		FSTreePtr GetChildNode( const FSTreePtr&    parent,
		                        const std::string&  name,
		                        const Key&          key ) const;
	};
	
	class FSTree_Volumes : public FSTree_Sequence< Volumes_Details >
	{
		private:
			typedef FSTree_Sequence< Volumes_Details > Base;
		
		public:
			FSTree_Volumes( const FSTreePtr&    parent,
			                const std::string&  name ) : Base( parent, name )
			{
			}
			
			ino_t Inode() const;
	};
	
	
	class FSTree_J_Symlink : public FSTree
	{
		public:
			FSTree_J_Symlink( const FSTreePtr& parent ) : FSTree( parent, const_j_directory_name )
			{
			}
			
			bool IsLink() const  { return true; }
			
			std::string ReadLink() const  { return "/"; }
			
			FSTreePtr ResolveLink() const  { return FSRoot(); }
	};
	
	FSTreePtr FSTreeFromFSSpec( const FSSpec& item )
	{
		return FSTreePtr( new FSTree_FSSpec( item ) );
	}
	
	static FSTreePtr FSTreeFromFSSpecRespectingJ( const FSSpec& item, const std::string& name )
	{
		if ( IsRootDirectory( item ) )
		{
			FSTreePtr parent( new FSTree_FSSpec( io::get_preceding_directory( item ) ) );
			
			return FSTreePtr( new FSTree_J_Symlink( parent ) );
		}
		
		return FSTreePtr( new FSTree_HFS( item, name ) );
	}
	
	
	static const FSTreePtr& MakeFSRoot()
	{
		FSTree_FSSpec* tree = NULL;
		
		static FSTreePtr result = FSTreePtr( tree = new FSTree_FSSpec( GetJDirectory() ) );
		
		if ( tree != NULL )
		{
			FSTreePtr users( new FSTree_FSSpec( io::system_root< N::FSDirSpec >() / "Users" ) );
			
			tree->Map( users );
			
			tree->Map( FSTreePtr( new FSTree_Volumes( result, "Volumes" ) ) );
			tree->Map( FSTreePtr( new FSTree_proc   ( result, "proc"    ) ) );
			
			tree->Map( Premapped_Factory< dev_Mappings >( result, "dev" ) );
			tree->Map( Premapped_Factory< new_Mappings >( result, "new" ) );
			tree->Map( Premapped_Factory< sys_Mappings >( result, "sys" ) );
		}
		
		return result;
	}
	
	const FSTreePtr& FSRoot()
	{
		static const FSTreePtr& root = MakeFSRoot();
		
		return root;
	}
	
	
	bool FSTree_HFS::Exists() const
	{
		return io::item_exists( itsFileSpec );
	}
	
	bool FSTree_HFS::IsFile() const
	{
		return io::file_exists( itsFileSpec );
	}
	
	bool FSTree_HFS::IsDirectory() const
	{
		return io::directory_exists( itsFileSpec );
	}
	
	bool FSTree_LongName::Exists() const
	{
		return ItemWithLongNameExists( itsParent, Name() );
	}
	
	bool FSTree_LongName::IsFile() const
	{
		return Exists() && io::file_exists( GetFSSpec() );
	}
	
	bool FSTree_LongName::IsDirectory() const
	{
		return Exists() && io::directory_exists( GetFSSpec() );
	}
	
	bool FSTree_HFS::IsLink() const
	{
		CInfoPBRec paramBlock;
		
		try
		{
			// GetFSSpec() may throw ENOENT for nonexistent long names
			FSSpec spec = GetFSSpec();
			
			try
			{
				FSpGetCatInfo( spec, paramBlock, Async() );
				
				const HFileInfo& hFileInfo = paramBlock.hFileInfo;
				
				bool isDir = hFileInfo.ioFlAttrib & kioFlAttribDirMask;
				
				bool isAlias = !isDir  &&  hFileInfo.ioFlFndrInfo.fdFlags & kIsAlias;
				
				return isAlias;
			}
			catch ( const N::FNFErr& err )
			{
			#ifdef __MWERKS__
				
				if ( err != fnfErr )
				{
					throw;
				}
				
			#endif
			}
		}
		catch ( const p7::errno_t& err )
		{
			if ( err != ENOENT )
			{
				throw;
			}
		}
		
		return false;
	}
	
	inline FSTreePtr Get_sys_mac_vol_N( N::FSVolumeRefNum vRefNum )
	{
		return sys_mac_vol_Details::GetChildNode( ResolvePathname( "/sys/mac/vol" ),
		                                          VRefNum_KeyName_Traits::NameFromKey( vRefNum ),
		                                          vRefNum );
	}
	
	FSTreePtr FSTree_HFS::Parent() const
	{
		if ( itsFileSpec.parID == fsRtParID )
		{
			return Get_sys_mac_vol_N( N::FSVolumeRefNum( itsFileSpec.vRefNum ) );
		}
		
		if ( IsRootDirectory( itsFileSpec ) || itsFileSpec == GetUsersDirectory() )
		{
			return FSRoot();
		}
		
		return FSTreePtr( new FSTree_FSSpec( io::get_preceding_directory( itsFileSpec ) ) );
	}
	
	FSTreePtr FSTree_LongName::Parent() const
	{
		return FSTreePtr( new FSTree_FSSpec( itsParent ) );
	}
	
	FSSpec FSTree_HFS::GetFSSpec( bool forCreation ) const
	{
		return itsFileSpec;
	}
	
	ino_t FSTree_HFS::ParentInode() const
	{
		return itsFileSpec.parID;
	}
	
	FSSpec FSTree_LongName::GetFSSpec( bool forCreation ) const
	{
		return FSSpecForLongUnixName( itsParent, Name(), forCreation );
	}
	
	ino_t FSTree_HFS::Inode() const
	{
		struct ::stat sb;
		
		Stat( sb );
		
		return sb.st_ino;
	}
	
	void FSTree_HFS::Stat( struct ::stat& sb ) const
	{
		StatFile( GetFSSpec(), &sb, false );
	}
	
	void FSTree_HFS::ChangeMode( mode_t mode ) const
	{
		ChangeFileMode( GetFSSpec(), mode );
	}
	
	void FSTree_HFS::SetTimes() const
	{
		using namespace TimeOff;
		
		UInt32 modTime = N::GetDateTime();
		
		FSSpec filespec = GetFSSpec();
		
		CInfoPBRec paramBlock;
		
		N::FSpGetCatInfo( GetFSSpec(), paramBlock );
		
		paramBlock.hFileInfo.ioNamePtr = filespec.name;
		paramBlock.hFileInfo.ioDirID   = filespec.parID;
		
		paramBlock.hFileInfo.ioFlMdDat = modTime;
		
		N::PBSetCatInfoSync( paramBlock );
	}
	
	void FSTree_HFS::SetTimes( const struct timeval* access,
	                           const struct timeval* mod,
	                           const struct timeval* backup,
	                           const struct timeval* creat ) const
	{
		using namespace TimeOff;
		
		FSSpec filespec = GetFSSpec();
		
		CInfoPBRec paramBlock;
		
		N::FSpGetCatInfo( GetFSSpec(), paramBlock );
		
		paramBlock.hFileInfo.ioNamePtr = filespec.name;
		paramBlock.hFileInfo.ioDirID   = filespec.parID;
		
		if ( creat )
		{
			paramBlock.hFileInfo.ioFlCrDat = creat->tv_sec + MacToUnixTimeDifference();
		}
		
		if ( mod )
		{
			paramBlock.hFileInfo.ioFlMdDat = mod->tv_sec + MacToUnixTimeDifference();
		}
		
		if ( backup )
		{
			paramBlock.hFileInfo.ioFlBkDat = backup->tv_sec + MacToUnixTimeDifference();
		}
		
		N::PBSetCatInfoSync( paramBlock );
	}
	
	void FSTree_HFS::Delete() const
	{
		FSSpec file = GetFSSpec();
		
		// returns fnfErr for directories
		OSErr unlockErr = ::FSpRstFLock( &file );
		
		OSErr deleteErr = ::FSpDelete( &file );
		
		// Unfortunately, fBsyErr can mean different things.
		// Here we assume it means a directory is not empty.
		
		if ( unlockErr == fnfErr  &&  deleteErr == fBsyErr )
		{
			p7::throw_errno( ENOTEMPTY );
		}
		
		N::ThrowOSStatus( deleteErr );
	}
	
	
	static const char* Basename( const char* path )
	{
		const char* slash = std::strrchr( path, '/' );
		
		if ( slash == NULL )
		{
			return path;
		}
		
		return slash + 1;
	}
	
	inline std::string UntweakMacFilename( std::string name )
	{
		std::replace( name.begin(), name.end(), ':', '/' );
		
		return name;
	}
	
	static bool NamesAreSame( ConstStr63Param a, ConstStr63Param b )
	{
		// b may be a null string to indicate a will be reused, so they're the same
		return b[0] == 0  ||  std::equal( a,
		                                  a + 1 + a[0],
		                                  b );
	}
	
	static bool FileIsLocked( const FSSpec& file )
	{
		CInfoPBRec cInfo;
		
		N::FSpGetCatInfo( file, cInfo );
		
		bool locked = cInfo.hFileInfo.ioFlAttrib & kioFlAttribLockedMask;
		
		return locked;
	}
	
	class FileLockBypass
	{
		private:
			FSSpec  itsFileSpec;
			bool    itWasLocked;
		
		public:
			FileLockBypass( const FSSpec& file ) : itsFileSpec( file ),
			                                       itWasLocked( FileIsLocked( file ) )
			{
				if ( itWasLocked )
				{
					N::FSpRstFLock( file );
				}
			}
			
			~FileLockBypass()
			{
				if ( itWasLocked )
				{
					OSErr err = ::FSpSetFLock( &itsFileSpec );
				}
			}
			
			void SetFile( const FSSpec& file )
			{
				itsFileSpec = file;
			}
	};
	
	static void RenameItem( const FSSpec& srcFile, const FSSpec& destFile )
	{
		ASSERT( srcFile.vRefNum == destFile.vRefNum );
		ASSERT( srcFile.parID   == destFile.parID   );
		
		FileLockBypass lockBypass( srcFile );
		
		// Rename source to dest
		N::FSpRename( srcFile, destFile.name );
		
		lockBypass.SetFile( destFile );
	}
	
	static void MoveAndRename( const FSSpec& srcFile, const FSSpec& destFile )
	{
		// Darn, we have to move *and* rename.  Use MoreFiles.
		
		// destFolder is the parent of destFile.
		// It's semantically invalid (though it might work) to pass an
		// FSSpec with null fields to Mac OS, but MoreFiles won't do that.
		// (It breaks it into individual parts before passing them.)
			
		FSSpec destFolder;
		
		destFolder.vRefNum = destFile.vRefNum;
		destFolder.parID   = destFile.parID;
		
		destFolder.name[0] = '\0';
		
		FileLockBypass lockBypass( srcFile );
		
		N::ThrowOSStatus( ::FSpMoveRenameCompat( &srcFile, &destFolder, destFile.name ) );
		
		lockBypass.SetFile( destFile );
	}
	
	static void SetLongName( const FSSpec& item, const std::string& name )
	{
		if ( name.length() > 31 )
		{
			N::FSpDTSetComment( item, name );
		}
	}
	
	static FSSpec GetFSSpecFromFSTree( const FSTreePtr& file )
	{
		struct ::stat stat_buffer;
		
		// Throws ENOENT if parent doesn't exist
		file->Parent()->Stat( stat_buffer );
		
		if ( stat_buffer.st_dev <= 0 )
		{
			// Not FSSpecified
			p7::throw_errno( EXDEV );
		}
		
		N::FSDirSpec parent;
		
		parent.vRefNum = N::FSVolumeRefNum( -stat_buffer.st_dev );
		parent.dirID   = N::FSDirID       (  stat_buffer.st_ino );
		
		return FSSpecForLongUnixName( parent, file->Name() );
	}
	
	void FSTree_HFS::Rename( const FSTreePtr& destFile ) const
	{
		const FSTree_HFS* srcFile = this;
		
		if ( !srcFile->Exists() )
		{
			p7::throw_errno( ENOENT );
		}
		
		bool destExists = destFile->Exists();
		
		bool srcIsDir  = srcFile ->IsDirectory();
		bool destIsDir = destFile->IsDirectory();
		
		if ( destExists  &&  srcIsDir != destIsDir )
		{
			p7::throw_errno( destIsDir ? EISDIR : ENOTDIR );
		}
		
		FSSpec srcFileSpec = srcFile->GetFSSpec( false );
		
		FSSpec destFileSpec = GetFSSpecFromFSTree( destFile );
		
		// Can't move across volumes
		if ( srcFileSpec.vRefNum != destFileSpec.vRefNum )
		{
			p7::throw_errno( EXDEV );
		}
		
		N::FSVolumeRefNum vRefNum = N::FSVolumeRefNum( srcFileSpec.vRefNum );
		
		const std::string& destName = destFile->Name();
		
		const bool keeping_name = std::equal( destName.begin(),
		                                      destName.end(),
		                                      (const char*) srcFileSpec.name + 1 );
		
		if ( srcFileSpec.parID == destFileSpec.parID )
		{
			// Same dir.
			
			if ( keeping_name )
			{
				// Identical names in the same directory.  Nothing to do.
				return;
			}
			
			if ( destExists )
			{
				const bool same_file = NamesAreSame( srcFileSpec.name, destFileSpec.name );
				
				if ( !same_file )
				{
					// Delete existing dest file
					N::FSpDelete( destFileSpec );
				}
				
				// Overwrite actual name with requested name
				
				std::string requested_name = K::MacFilenameFromUnixFilename( destName );
				
				N::CopyToPascalString( requested_name, destFileSpec.name, 31 );
			}
			
			RenameItem( srcFileSpec, destFileSpec );
			
			SetLongName( destFileSpec, destName );
			
			// And we're done
			return;
		}
		
		if ( destExists )
		{
			// Can't be the same file, because it's in a different directory.
			
			N::FSpDelete( destFileSpec );
		}
		
		if ( keeping_name )
		{
			// Same name, different dir; move only.
			N::FSpCatMove( srcFileSpec, N::FSDirID( destFileSpec.parID ) );
		}
		else
		{
			// Darn, we have to move *and* rename.
			MoveAndRename( srcFileSpec, destFileSpec );
			
			SetLongName( destFileSpec, destName );
		}
	}
	
	
	off_t FSTree_HFS::GetEOF() const
	{
		CInfoPBRec paramBlock;
		
		N::FSpGetCatInfo( GetFSSpec(), paramBlock );
		
		return paramBlock.hFileInfo.ioFlLgLen;
	}
	
	void FSTree_HFS::SetEOF( off_t length ) const
	{
		N::SetEOF( N::FSpOpenDF( GetFSSpec(), N::fsWrPerm ), length );
	}
	
	std::string FSTree_HFS::ReadLink() const
	{
		if ( !IsLink() )
		{
			p7::throw_errno( EINVAL );
		}
		
		return ResolveLink()->Pathname();
	}
	
	FSTreePtr FSTree_HFS::ResolveLink() const
	{
		if ( !Exists() )
		{
			return shared_from_this();
		}
		
		FSSpec target = N::ResolveAliasFile( GetFSSpec(), false );
		
		return FSTreePtr( new FSTree_FSSpec( target ) );
	}
	
	static N::FileSignature GetFileSignatureForAlias( const FSSpec& item )
	{
		if ( io::directory_exists( item ) )
		{
			return N::FileSignature( N::OSType( 'MACS'                    ),
			                         N::OSType( kContainerFolderAliasType ) );
		}
		
		FInfo fInfo = N::FSpGetFInfo( item );
		
		return N::FileSignature( fInfo );
	}
	
	static void CreateSymLink( const FSTreePtr& linkFile, const std::string& targetPath )
	{
		FSSpec linkSpec = linkFile->GetFSSpec( true );
		
		N::FSDirSpec linkParent = io::get_preceding_directory( linkSpec );
		
		FSSpec linkParentSpec = NN::Convert< FSSpec >( linkParent );
		
		// Target pathname is resolved relative to the location of the link file
		FSTreePtr target = ResolvePathname( targetPath, FSTreeFromFSSpec( linkParentSpec ) );
		
		// Do not resolve links -- if the target of this link is another symlink, so be it
		
		FSSpec targetSpec = target->GetFSSpec( false );
		
		N::FileSignature signature = GetFileSignatureForAlias( targetSpec );
		
		N::FSpCreateResFile( linkSpec, signature );
		
		NN::Owned< N::AliasHandle > alias = N::NewAlias( linkSpec, targetSpec );
		
		FInfo linkFInfo = N::FSpGetFInfo( linkSpec );
		
		linkFInfo.fdFlags |= kIsAlias;
		
		N::FSpSetFInfo( linkSpec, linkFInfo );
		
		NN::Owned< N::ResFileRefNum > aliasResFile = N::FSpOpenResFile( linkSpec, N::fsRdWrPerm );
		
		(void) N::AddResource< N::rAliasType >( alias, N::ResID( 0 ), "\p" );
	}
	
	void FSTree_HFS::SymLink( const std::string& target ) const
	{
		CreateSymLink( shared_from_this(), target );
	}
	
	void FSTree_HFS::CreateFile() const
	{
		// No need to convert name -- for examination only
		std::string name = io::get_filename_string( itsFileSpec );
		
		N::FileSignature sig = PickFileSignatureForName( name );
		
		N::FSpCreate( itsFileSpec, sig );
	}
	
	void FSTree_LongName::CreateFile() const
	{
		CreateFileWithLongName( itsParent, Name() );
	}
	
	boost::shared_ptr< IOHandle > FSTree_HFS::Open( OpenFlags flags, mode_t mode ) const
	{
		bool creating  = flags & O_CREAT;
		bool excluding = flags & O_EXCL;
		
		bool created = false;
		
		if ( creating )
		{
			CInfoPBRec cInfo;
			
			N::Str255 actualName = itsFileSpec.name;
			
			NN::Initialize< CInfoPBRec >( cInfo,
			                              N::FSVolumeRefNum( itsFileSpec.vRefNum ),
			                              N::FSDirID       ( itsFileSpec.parID   ),
			                              actualName,
			                              0 );
			
			const bool exists = N::PBGetCatInfoSync( cInfo, N::FNF_Returns() );
			
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
				const std::string& name = Name();
				
				const bool equal = std::equal( name.begin(),
				                               name.end(),
				                               (const char*) (actualName + 1) );
				
				if ( !equal )
				{
					std::string new_mac_name = K::MacFilenameFromUnixFilename( name );
					
					N::FSpRename( itsFileSpec, new_mac_name );
				}
			}
		}
		
		boost::shared_ptr< IOHandle > opened = Open( flags );
		
		if ( created  &&  (mode & 0200) == 0 )
		{
			N::FSpSetFLock( GetFSSpec() );
		}
		
		return opened;
	}
	
	boost::shared_ptr< IOHandle > FSTree_HFS::Open( OpenFlags flags ) const
	{
		return DataForkUser().OpenFileHandle( GetFSSpec(), flags );
	}
	
	MainEntry FSTree_HFS::GetMainEntry() const
	{
		return GetMainEntryFromFile( GetFSSpec() );
	}
	
	void FSTree_HFS::CreateDirectory( mode_t /*mode*/ ) const
	{
		N::FSpDirCreate( itsFileSpec );
	}
	
	void FSTree_LongName::CreateDirectory( mode_t /*mode*/ ) const
	{
		CreateDirectoryWithLongName( itsParent, Name() );
	}
	
	FSTreePtr FSTree_HFS::Lookup_Regular( const std::string& name ) const
	{
		FSSpec thisFSSpec = GetFSSpec();
		
		if ( name == "rsrc"  &&  IsFile() )
		{
			return GetRsrcForkFSTree( thisFSSpec );
		}
		
		if ( name.size() > 31 )
		{
			return FSTreePtr( new FSTree_LongName( N::FSDirSpec( thisFSSpec ), name ) );
		}
		
		std::string macName = MacFromUnixName( name );
		
		FSSpec item = thisFSSpec / macName;
		
		return FSTreeFromFSSpecRespectingJ( item, name );
	}
	
	void FSTree_HFS::IterateIntoCache( FSTreeCache& cache ) const
	{
		CInfoPBRec pb;
		
		N::FSpGetCatInfo( GetFSSpec(), pb );
		
		N::FSDirSpec dir;
		
		dir.vRefNum = N::FSVolumeRefNum( pb.dirInfo.ioVRefNum );
		dir.dirID   = N::FSDirID       ( pb.dirInfo.ioDrDirID );
		
		std::string unixName;
		
		const UInt16 n_items = pb.dirInfo.ioDrNmFls;
		
		for ( UInt16 i = 1;  ;  ++i )
		{
			::Str255 name;
			
			if ( !N::FSpGetCatInfo( dir, i, pb, name, N::FNF_Returns() ) )
			{
				return;
			}
			
			unixName = GetUnixName( dir / name );
			
			ino_t inode = pb.hFileInfo.ioDirID;  // file or dir ID for inode
			
			FSNode node( inode, unixName );
			
			cache.push_back( node );
		}
	}
	
	
	class FSTree_Volumes_Link : public FSTree, public Volume_KeyName_Traits
	{
		private:
			Key itsKey;
		
		public:
			FSTree_Volumes_Link( const FSTreePtr&    parent,
			                     const std::string&  name,
			                     const Key&          key ) : FSTree( parent, name ),
			                                                 itsKey( key    )
			{
			}
			
			bool IsLink() const  { return true; }
			
			std::string ReadLink() const  { return ResolveLink()->Pathname(); }
			
			FSTreePtr ResolveLink() const  { return FSTreePtr( new FSTree_FSSpec( FSSpecFromKey( itsKey ) ) ); }
	};
	
	
	FSTreePtr Volumes_Details::GetChildNode( const FSTreePtr&    parent,
		                                     const std::string&  name,
		                                     const Key&          key ) const
	{
		return FSTreePtr( new FSTree_Volumes_Link( parent, name, key ) );
	}
	
	ino_t FSTree_Volumes::Inode() const
	{
		return fsRtParID;
	}
	
}

