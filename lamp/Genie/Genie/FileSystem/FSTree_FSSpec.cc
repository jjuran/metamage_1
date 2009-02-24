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

// Iota
#include "iota/strings.hh"

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

// Pedestal
#include "Pedestal/Application.hh"

// Genie
#include "Genie/FileSignature.hh"
#include "Genie/FileSystem/FSSpec.hh"
#include "Genie/FileSystem/FSSpecForkUser.hh"
#include "Genie/FileSystem/FSTree_Dev.hh"
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/FileSystem/FSTree_Proc.hh"
#include "Genie/FileSystem/FSTree_RsrcFile.hh"
#include "Genie/FileSystem/FSTree_new.hh"
#include "Genie/FileSystem/FSTree_sys.hh"
#include "Genie/FileSystem/FSTree_sys_mac_errata.hh"
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
	namespace Ped = Pedestal;
	
	
	namespace path_descent_operators
	{
		
		inline FSSpec operator/( const N::FSDirSpec& dir, const unsigned char* name )
		{
			return Genie::FSMakeFSSpec< FNF_Returns >( dir, name );
		}
		
		inline FSSpec operator/( const FSSpec& dir, const unsigned char* name )
		{
			return Dir_From_FSSpec( dir ) / name;
		}
		
		inline FSSpec operator/( const N::FSDirSpec& dir, const std::string& name )
		{
			return dir / N::Str63( name );
		}
		
		inline FSSpec operator/( const FSSpec& dir, const std::string& name )
		{
			return dir / N::Str63( name );
		}
		
	}
	
	using namespace path_descent_operators;
	
	
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
	
	
	static std::string GetUnixName( const FSSpec& item )
	{
		if ( item.name[0] == 31 )
		{
			try
			{
				std::string comment = N::FSpDTGetComment( item );
				
				if ( comment.size() > 31 )
				{
					std::string hashed = K::MacFilenameFromUnixFilename( comment );
					
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
	
	
	static void SetFileTimes( N::FSVolumeRefNum     vRefNum,
	                          N::FSDirID            dirID,
	                          const unsigned char*  name )
	{
		using namespace TimeOff;
		
		UInt32 modTime = N::GetDateTime();
		
		N::Str63 name_copy = name;
		
		CInfoPBRec paramBlock;
		
		FSpGetCatInfo< FNF_Throws >( paramBlock, vRefNum, dirID, name_copy );
		
		paramBlock.hFileInfo.ioDirID = dirID;
		
		paramBlock.hFileInfo.ioFlMdDat = modTime;
		
		N::PBSetCatInfoSync( paramBlock );
	}
	
	static void SetFileTimes( N::FSVolumeRefNum      vRefNum,
	                          N::FSDirID             dirID,
	                          const unsigned char*   name,
	                          const struct timeval*  access,
	                          const struct timeval*  mod,
	                          const struct timeval*  backup,
	                          const struct timeval*  creat )
	{
		using namespace TimeOff;
		
		N::Str63 name_copy = name;
		
		CInfoPBRec paramBlock;
		
		FSpGetCatInfo< FNF_Throws >( paramBlock, vRefNum, dirID, name_copy );
		
		paramBlock.hFileInfo.ioDirID = dirID;
		
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
	
	
	static const char* const_j_directory_name = "j";
	
	static FSSpec FindJDirectory()
	{
		// Try current directory first
		FSSpec j =  N::FSDirSpec() / const_j_directory_name;
		
		if ( !io::directory_exists( j ) )
		{
			// Then root, or bust
			j = io::system_root< N::FSDirSpec >() / const_j_directory_name;
			
			(void) Dir_From_FSSpec( j );  // throws if not a dir
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
		
		(void) Dir_From_FSSpec( users );  // throws if not a dir
		
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
			return FSMakeFSSpec< FNF_Throws >( key, N::fsRtDirID, "\p" );
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
	
	
	class FSTree_Root : public FSTree_Mappable
	{
		public:
			FSTree_Root();
			
			bool IsFile     () const  { return false; }
			bool IsDirectory() const  { return true;  }
			
			FSTreePtr Parent() const  { return shared_from_this(); }
			
			ino_t ParentInode() const  { return Inode(); }
			
			void Stat( struct ::stat& sb ) const;
			
			void SetTimes() const;
			
			void SetTimes( const struct timeval* access,
			               const struct timeval* mod,
			               const struct timeval* backup,
			               const struct timeval* creat ) const;
			
			ino_t Inode() const;
			
			FSTreePtr Lookup_Regular( const std::string& name ) const;
			
			void IterateIntoCache( FSTreeCache& cache ) const;
	};
	
	FSTree_Root::FSTree_Root()
	:
		FSTree_Mappable( FSTreePtr(), "" )
	{
		// we override Parent()
	}
	
	ino_t FSTree_Root::Inode() const
	{
		struct ::stat sb;
		
		Stat( sb );
		
		return sb.st_ino;
	}
	
	void FSTree_Root::Stat( struct ::stat& sb ) const
	{
		StatFile( GetJDirectory(), &sb, false );
	}
	
	void FSTree_Root::SetTimes() const
	{
		const FSSpec& root = GetJDirectory();
		
		SetFileTimes( N::FSVolumeRefNum( root.vRefNum ),
		              N::FSDirID       ( root.parID   ),
		              root.name );
	}
	
	void FSTree_Root::SetTimes( const struct timeval* access,
	                            const struct timeval* mod,
	                            const struct timeval* backup,
	                            const struct timeval* creat ) const
	{
		const FSSpec& root = GetJDirectory();
		
		SetFileTimes( N::FSVolumeRefNum( root.vRefNum ),
		              N::FSDirID       ( root.parID   ),
		              root.name,
		              access,
		              mod,
		              backup,
		              creat );
	}
	
	
	class FSTree_HFS : public FSTree_Directory
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
			
			const FSSpec& GetFSSpec() const  { return itsFileSpec; }
			
			ino_t ParentInode() const  { return itsFileSpec.parID; }
			
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
			
			FSTreePtr Lookup_Child( const std::string& name ) const;
			
			void IterateIntoCache( FSTreeCache& cache ) const;
			
			FSTreePtr ResolvePath( const char*& begin, const char* end ) const;
		
		private:
			void CreateFile() const;
			
			void FinishCreation() const;
	};
	
	FSTree_HFS::FSTree_HFS( const FSSpec&       file,
	                        const std::string&  name )
	:
		FSTree_Directory( FSTreePtr(), name.empty() ? MakeName( file )
		                                            : name ),
		itsFileSpec     ( file                             )
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
		
		FSpFileCopy( srcFile, destDir / "", name );
	}
	
	
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
			
			ino_t Inode() const  { return fsRtParID; }
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
		return FSTreePtr( new FSTree_HFS( item ) );
	}
	
	
	static const FSTreePtr& MakeFSRoot()
	{
		FSTree_Root* tree = NULL;
		
		static FSTreePtr result = FSTreePtr( tree = new FSTree_Root() );
		
		if ( tree != NULL )
		{
			tree->Map( FSTreeFromFSSpec( GetUsersDirectory() ) );
			
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
	
	bool FSTree_HFS::IsLink() const
	{
		CInfoPBRec paramBlock;
		
		if ( FSpGetCatInfo< FNF_Returns >( paramBlock, itsFileSpec ) )
		{
			const HFileInfo& hFileInfo = paramBlock.hFileInfo;
			
			const bool isDir = hFileInfo.ioFlAttrib & kioFlAttribDirMask;
			
			const bool isAlias = !isDir  &&  hFileInfo.ioFlFndrInfo.fdFlags & kIsAlias;
			
			return isAlias;
		}
		
		return false;
	}
	
	inline FSTreePtr Get_sys_mac_vol_N( N::FSVolumeRefNum vRefNum )
	{
		return sys_mac_vol_Details::GetChildNode( ResolveAbsolutePath( STR_LEN( "/sys/mac/vol" ) ),
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
		
		return FSTreePtr( FSTreeFromFSDirSpec( io::get_preceding_directory( itsFileSpec ) ) );
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
		SetFileTimes( N::FSVolumeRefNum( itsFileSpec.vRefNum ),
		              N::FSDirID       ( itsFileSpec.parID   ),
		              itsFileSpec.name );
	}
	
	void FSTree_HFS::SetTimes( const struct timeval* access,
	                           const struct timeval* mod,
	                           const struct timeval* backup,
	                           const struct timeval* creat ) const
	{
		SetFileTimes( N::FSVolumeRefNum( itsFileSpec.vRefNum ),
		              N::FSDirID       ( itsFileSpec.parID   ),
		              itsFileSpec.name,
		              access,
		              mod,
		              backup,
		              creat );
	}
	
	void FSTree_HFS::Delete() const
	{
		// returns fnfErr for directories
		OSErr unlockErr = ::FSpRstFLock( &itsFileSpec );
		
		OSErr deleteErr = ::FSpDelete( &itsFileSpec );
		
		// Unfortunately, fBsyErr can mean different things.
		// Here we assume it means a directory is not empty.
		
		if ( unlockErr == fnfErr  &&  deleteErr == fBsyErr )
		{
			p7::throw_errno( ENOTEMPTY );
		}
		
		N::ThrowOSStatus( deleteErr );
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
		
		FSpGetCatInfo< FNF_Throws >( cInfo, file );
		
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
	
	
	FSSpec GetFSSpecFromFSTree( const FSTreePtr& file )
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
		
		return parent / K::MacFilenameFromUnixFilename( file->Name() );
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
		
		const FSSpec& srcFileSpec = itsFileSpec;
		
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
		
		FSpGetCatInfo< FNF_Throws >( paramBlock, GetFSSpec() );
		
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
		CInfoPBRec cInfo;
		
		const bool exists = FSpGetCatInfo< FNF_Returns >( cInfo, GetFSSpec() );
		
		if ( !exists )
		{
			//return shared_from_this();
		}
		else if ( cInfo.hFileInfo.ioFlFndrInfo.fdFlags & kIsAlias )
		{
			FSSpec target = N::ResolveAliasFile( GetFSSpec(), false );
			
			return FSTreeFromFSSpec( target );
		}
		
		return shared_from_this();
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
		FSSpec linkSpec = GetFSSpecFromFSTree( linkFile );
		
		N::FSDirSpec linkParent = io::get_preceding_directory( linkSpec );
		
		// Target pathname is resolved relative to the location of the link file
		FSTreePtr target = ResolvePathname( targetPath, FSTreeFromFSDirSpec( linkParent ) );
		
		// Do not resolve links -- if the target of this link is another symlink, so be it
		
		FSSpec targetSpec = GetFSSpecFromFSTree( target );
		
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
		N::FileSignature sig = PickFileSignatureForName( Name() );
		
		N::FSpCreate( itsFileSpec, sig );
		
		FinishCreation();
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
			
			const bool exists = FSpGetCatInfo< FNF_Returns >( cInfo,
			                                                  N::FSVolumeRefNum( itsFileSpec.vRefNum ),
			                                                  N::FSDirID       ( itsFileSpec.parID   ),
			                                                  actualName,
			                                                  0 );
			
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
	
	static inline MainEntry GetMainEntryFromFile( const FSSpec& file )
	{
		BinaryImage image = GetBinaryImage( file );
		
		return GetMainEntryFromBinaryImage( image );
	}
	
	MainEntry FSTree_HFS::GetMainEntry() const
	{
		return GetMainEntryFromFile( GetFSSpec() );
	}
	
	void FSTree_HFS::CreateDirectory( mode_t /*mode*/ ) const
	{
		N::FSpDirCreate( itsFileSpec );
		
		FinishCreation();
	}
	
	
	FSTreePtr FSTree_Root::Lookup_Regular( const std::string& name ) const
	{
		N::FSDirSpec dir = Dir_From_FSSpec( GetJDirectory() );
		
		const std::string macName = K::MacFilenameFromUnixFilename( name );
		
		const FSSpec item = dir / macName;
		
		return FSTreePtr( new FSTree_HFS( item, name ) );
	}
	
	FSTreePtr FSTree_HFS::Lookup_Child( const std::string& name ) const
	{
		if ( name == "rsrc"  &&  IsFile() )
		{
			return GetRsrcForkFSTree( itsFileSpec );
		}
		
		N::FSDirSpec dir = Dir_From_FSSpec( itsFileSpec );
		
		const std::string macName = K::MacFilenameFromUnixFilename( name );
		
		const FSSpec item = dir / macName;
		
		const bool is_root = IsRootDirectory( item );
		
		typedef FSTree* Ptr;
		
		Ptr ptr = is_root ? Ptr( new FSTree_J_Symlink( shared_from_this() ) )
		                  : Ptr( new FSTree_HFS      ( item, name         ) );
		
		return FSTreePtr( ptr );
	}
	
	static void IterateFilesIntoCache( CInfoPBRec&   pb,
	                                   FSTreeCache&  cache )
	{
		N::FSVolumeRefNum vRefNum = N::FSVolumeRefNum( pb.dirInfo.ioVRefNum );
		N::FSDirID        dirID   = N::FSDirID       ( pb.dirInfo.ioDrDirID );
		
		for ( UInt16 i = 1;  ;  ++i )
		{
			FSSpec item = { vRefNum, dirID, "\p" };
			
			if ( !FSpGetCatInfo< FNF_Returns >( pb, vRefNum, dirID, item.name, i ) )
			{
				return;
			}
			
			const ino_t inode = pb.hFileInfo.ioDirID;  // file or dir ID for inode
			
			const FSNode node( inode, GetUnixName( item ) );
			
			cache.push_back( node );
		}
	}
	
	void FSTree_Root::IterateIntoCache( FSTreeCache& cache ) const
	{
		CInfoPBRec cInfo;
		
		FSpGetCatInfo< FNF_Throws >( cInfo, GetJDirectory() );
		
		IterateFilesIntoCache( cInfo, cache );
	}
	
	void FSTree_HFS::IterateIntoCache( FSTreeCache& cache ) const
	{
		CInfoPBRec cInfo;
		
		FSpGetCatInfo< FNF_Throws >( cInfo, GetFSSpec() );
		
		IterateFilesIntoCache( cInfo, cache );
	}
	
	struct ResolvePath_CInfoPBRec : CInfoPBRec
	{
		N::Str63       name;
		const char*    begin;
		const char*    end;
		volatile bool  done;
	};
	
	static bool name_is_special( const char* begin, const char* end )
	{
		const unsigned name_length = end - begin;
		
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
		else if ( std::find( begin, end, ':' ) != end )
		{
			return true;
		}
		
		return false;
	}
	
	static pascal void ResolvePath_Completion( ParamBlockRec* pb )
	{
		ResolvePath_CInfoPBRec& cInfo = *(ResolvePath_CInfoPBRec*) pb;
		
		if ( cInfo.dirInfo.ioResult < 0 )
		{
			goto done;
		}
		
		const char* begin = cInfo.begin;
		
		if ( begin == cInfo.end )
		{
			goto done;
		}
		
		const bool is_dir = cInfo.hFileInfo.ioFlAttrib & kioFlAttribDirMask;
		
		if ( !is_dir )
		{
			const bool is_alias = cInfo.hFileInfo.ioFlFndrInfo.fdFlags & kIsAlias;
			
			if ( !is_alias )
			{
				// I wanted a dir but you gave me a file.  You creep.
				cInfo.dirInfo.ioResult = errFSNotAFolder;
			}
			
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
		
		cInfo.begin = slash + (slash != cInfo.end);
		
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
	
#if TARGET_CPU_68K && !TARGET_RT_MAC_CFM
	
	static pascal asm void ResolvePath_Completion_68K()
	{
		MOVE.L (SP),-(SP)
		MOVE.L A0,4(SP)
		BRA    ResolvePath_Completion
	}
	
	static ::IOCompletionUPP gResolvePathCompletion = ResolvePath_Completion_68K;
	
#else
	
	static ::IOCompletionUPP gResolvePathCompletion = ::NewIOCompletionUPP( ResolvePath_Completion );
	
#endif
	
	FSTreePtr FSTree_HFS::ResolvePath( const char*& begin, const char* end ) const
	{
		if (     name_is_special( begin, std::find( begin, end, '/' ) )
		     ||  RunningInClassic::Test() )
		{
			// Special handling required for
			// * dot, dotdot, colons, and long names
			// * running in Classic, which has broken PBGetCatInfoAsync()
			
			return FSTree::ResolvePath( begin, end );
		}
		
		if ( begin == end )
		{
			return shared_from_this();
		}
		
		ASSERT( begin < end );
		
		ResolvePath_CInfoPBRec cInfo;
		
		DirInfo& dirInfo = cInfo.dirInfo;
		
		dirInfo.ioCompletion = gResolvePathCompletion;
		
		dirInfo.ioNamePtr   = cInfo.name;
		dirInfo.ioVRefNum   = itsFileSpec.vRefNum;
		dirInfo.ioDrDirID   = itsFileSpec.parID;
		dirInfo.ioFDirIndex = 0;
		
		cInfo.name = itsFileSpec.name;
		
		cInfo.begin = begin;
		cInfo.end   = end;
		cInfo.done  = false;
		
		N::PBGetCatInfoAsync( cInfo, FNF_Returns() );
		
		while ( !cInfo.done )
		{
			AsyncYield();
		}
		
		begin = cInfo.begin;
		
		ASSERT( dirInfo.ioResult <= 0 );
		
		const bool nonexistent = dirInfo.ioResult == fnfErr  && begin == end;
		
		if ( !nonexistent )
		{
			N::ThrowOSStatus( dirInfo.ioResult );
		}
		
		const SInt32 dirID = nonexistent ? dirInfo.ioDrDirID : dirInfo.ioDrParID;
		
		FSSpec result = { dirInfo.ioVRefNum, dirID };
		
		N::CopyToPascalString( cInfo.name, result.name, sizeof result.name - 1 );
		
		return FSTreeFromFSSpec( result );
	}
	
	void FSTree_HFS::FinishCreation() const
	{
		SetLongName( itsFileSpec, Name() );
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
			
			FSTreePtr ResolveLink() const  { return FSTreeFromFSSpec( FSSpecFromKey( itsKey ) ); }
	};
	
	
	FSTreePtr Volumes_Details::GetChildNode( const FSTreePtr&    parent,
		                                     const std::string&  name,
		                                     const Key&          key ) const
	{
		return FSTreePtr( new FSTree_Volumes_Link( parent, name, key ) );
	}
	
}

