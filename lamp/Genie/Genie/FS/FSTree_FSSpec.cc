/*	================
 *	FSTree_FSSpec.cc
 *	================
 */

#include "Genie/FS/FSTree_FSSpec.hh"

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

// Debug
#include "debug/assert.hh"

// Nitrogen
#include "Nitrogen/Aliases.h"
#include "Nitrogen/Files.h"
#include "Nitrogen/Resources.h"

// Io: MacFiles
#include "MacFiles.hh"

// MacIO
#include "MacIO/FSMakeFSSpec_Sync.hh"
#include "MacIO/GetCatInfo_Sync.hh"

// poseven
#include "poseven/Errno.hh"

// TimeOff
#include "TimeOff.hh"

// Arcana / MD5
#include "MD5.hh"

// Kerosene
#include "MacFilenameFromUnixFilename.hh"

// Pedestal
#include "Pedestal/Application.hh"

// Genie
#include "Genie/Exec/GetMainEntry.hh"
#include "Genie/FileSignature.hh"
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/FSSpecForkUser.hh"
#include "Genie/FS/FSTree_Dev.hh"
#include "Genie/FS/FSTree_Directory.hh"
#include "Genie/FS/FSTree_Proc.hh"
#include "Genie/FS/FSTree_RsrcFile.hh"
#include "Genie/FS/FSTree_new.hh"
#include "Genie/FS/FSTree_sys.hh"
#include "Genie/FS/FSTree_sys_mac_errata.hh"
#include "Genie/FS/FSTree_sys_mac_vol.hh"
#include "Genie/FS/ResFile_Dir.hh"
#include "Genie/FS/ResolvePathname.hh"
#include "Genie/FS/StatFile.hh"
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
			return MacIO::FSMakeFSSpec< FNF_Returns >( dir, name );
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
	
	
	static N::FSVolumeRefNum GetVRefNum( N::FSVolumeRefNum  vRefNum = N::FSVolumeRefNum() )
	{
		HParamBlockRec pb;
		
		pb.volumeParam.ioVRefNum  = vRefNum;
		pb.volumeParam.ioNamePtr  = NULL;
		pb.volumeParam.ioVolIndex = 0;  // use ioVRefNum only
		
		N::ThrowOSStatus( ::PBHGetVInfoSync( &pb ) );
		
		return N::FSVolumeRefNum( pb.volumeParam.ioVRefNum );
	}
	
	static N::FSVolumeRefNum GetVRefNum( const std::string& name )
	{
		N::Str255 name_copy = name;
		
		HParamBlockRec pb;
		
		pb.volumeParam.ioVRefNum  = 0;
		pb.volumeParam.ioNamePtr  = name_copy;
		pb.volumeParam.ioVolIndex = -1;  // use use ioNamePtr/ioVRefNum combination
		
		N::ThrowOSStatus( ::PBHGetVInfoSync( &pb ) );
		
		return N::FSVolumeRefNum( pb.volumeParam.ioVRefNum );
	}
	
	
	static void SetFileTimes( N::FSVolumeRefNum     vRefNum,
	                          N::FSDirID            dirID,
	                          const unsigned char*  name )
	{
		UInt32 modTime = N::GetDateTime();
		
		N::Str63 name_copy = name;
		
		CInfoPBRec paramBlock;
		
		MacIO::GetCatInfo< FNF_Throws >( paramBlock, vRefNum, dirID, name_copy );
		
		paramBlock.hFileInfo.ioDirID = dirID;
		
		paramBlock.hFileInfo.ioFlMdDat = modTime;
		
		N::PBSetCatInfoSync( paramBlock );
	}
	
	static void update_time( UInt32& date, const timespec& time, UInt32& now )
	{
		const long nsec = time.tv_nsec & ~UTIME_ARCHIVE;
		
		if ( nsec == UTIME_OMIT )
		{
			return;
		}
		
		if ( nsec == UTIME_NOW )
		{
			if ( now == 0 )
			{
				now = N::GetDateTime();
			}
			
			date = now;
		}
		else
		{
			using namespace TimeOff;
			
			date = time.tv_sec + MacToUnixTimeDifference();
		}
	}
	
	static void SetFileTimes( N::FSVolumeRefNum       vRefNum,
	                          N::FSDirID              dirID,
	                          const unsigned char    *name,
	                          const struct timespec   times[2] )
	{
		const timespec& atime = times[0];
		const timespec& mtime = times[1];
		
		const bool atime_is_backup = atime.tv_nsec & UTIME_ARCHIVE;
		
		using namespace TimeOff;
		
		N::Str63 name_copy = name;
		
		CInfoPBRec paramBlock;
		
		MacIO::GetCatInfo< FNF_Throws >( paramBlock, vRefNum, dirID, name_copy );
		
		paramBlock.hFileInfo.ioDirID = dirID;
		
		UInt32 now = 0;
		
		//paramBlock.hFileInfo.ioFlCrDat
		
		update_time( paramBlock.hFileInfo.ioFlMdDat, mtime, now );
		
		if ( atime_is_backup )
		{
			update_time( paramBlock.hFileInfo.ioFlBkDat, atime, now );
		}
		
		N::PBSetCatInfoSync( paramBlock );
	}
	
	
	static const unsigned char* const_root_directory_name = "\p" "j";
	
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
	
	static const N::FSDirSpec& GetJDirectory()
	{
		static N::FSDirSpec j = FindJDirectory();
		
		return j;
	}
	
	static N::FSDirSpec FindUsersDirectory()
	{
		N::FSDirSpec root = io::system_root< N::FSDirSpec >();
		
		FSSpec users = root / "Users";
		
		return Dir_From_FSSpec( users );
	}
	
	static const N::FSDirSpec& GetUsersDirectory()
	{
		static N::FSDirSpec users = FindUsersDirectory();
		
		return users;
	}
	
	
	struct Volume_KeyName_Traits
	{
		typedef N::FSVolumeRefNum Key;
		
		static FSSpec FSSpecFromKey( const Key& key )
		{
			return MacIO::FSMakeFSSpec< FNF_Throws >( key, N::fsRtDirID, "\p" );
		}
		
		static std::string NameFromKey( const Key& key )
		{
			return UnixFromMacName( io::get_filename_string( FSSpecFromKey( key ) ) );
		}
		
		static Key KeyFromName( const std::string& name )
		{
			Key key = GetVRefNum( MacFromUnixName( name ) + ":" );
			
			return key;
		}
	};
	
	
	static std::string MakeName( const FSSpec& fileSpec )
	{
		if ( fileSpec.parID == fsRtParID )
		{
			return "mnt";
		}
		
		return GetUnixName( fileSpec );
	}
	
	
	class FSTree_Root : public FSTree_Mappable
	{
		public:
			FSTree_Root();
			
			bool IsFile     () const  { return false; }
			bool IsDirectory() const  { return true;  }
			
			FSTreePtr Parent() const  { return Self(); }
			
			ino_t ParentInode() const  { return Inode(); }
			
			void Stat( struct ::stat& sb ) const;
			
			void SetTimes() const;
			
			void SetTimes( const struct timespec times[2] ) const;
			
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
		const N::FSDirSpec& root = GetJDirectory();
		
		Stat_HFS( false, &sb, root.vRefNum, root.dirID );
	}
	
	void FSTree_Root::SetTimes() const
	{
		const N::FSDirSpec& root = GetJDirectory();
		
		SetFileTimes( root.vRefNum, root.dirID, NULL );
	}
	
	void FSTree_Root::SetTimes( const struct timespec times[2] ) const
	{
		const N::FSDirSpec& root = GetJDirectory();
		
		SetFileTimes( root.vRefNum,
		              root.dirID,
		              NULL,
		              times );
	}
	
	
	class FSTree_DirSpec : public FSTree_Directory
	{
		private:
			N::FSDirSpec  itsDirSpec;
			bool          itIsOnServer;
		
		public:
			FSTree_DirSpec( const N::FSDirSpec&  dir,
			                bool                 onServer,
			                const std::string&   name );
			
			bool IsFile     () const  { return false; }
			bool IsDirectory() const  { return true;  }
			
			FSTreePtr Parent() const;
			
			void Stat( struct ::stat& sb ) const;
			
			void SetTimes() const;
			
			void SetTimes( const struct timespec times[2] ) const;
			
			void Delete() const;
			
			void Rename( const FSTreePtr& destination ) const;
			
			ino_t Inode() const  { return itsDirSpec.dirID; }
			
			FSTreePtr Lookup_Child( const std::string& name ) const;
			
			void IterateIntoCache( FSTreeCache& cache ) const;
	};
	
	FSTree_DirSpec::FSTree_DirSpec( const N::FSDirSpec&  dir,
	                                bool                 onServer,
	                                const std::string&   name )
	:
		FSTree_Directory( FSTreePtr(), name ),
		itIsOnServer    ( onServer          ),
		itsDirSpec      ( dir               )
	{
		// we override Parent()
	}
	
	
	class FSTree_HFS : public FSTree_Directory
	{
		private:
			FSSpec  itsFileSpec;
			bool    itIsOnServer;
		
		public:
			FSTree_HFS( const FSSpec&       file,
			            bool                onServer,
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
			
			void SetTimes( const struct timespec times[2] ) const;
			
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
	                        bool                onServer,
	                        const std::string&  name )
	:
		FSTree_Directory( FSTreePtr(), name.empty() ? MakeName( file )
		                                            : name ),
		itsFileSpec     ( file                             ),
		itIsOnServer    ( onServer                         )
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
			return true;  // GetVRefNum() only returns valid keys
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
	
	
	FSTreePtr FSTreeFromFSSpec( const FSSpec& item, bool onServer )
	{
		return FSTreePtr( new FSTree_HFS( item, onServer ) );
	}
	
	FSTreePtr FSTreeFromFSDirSpec( const N::FSDirSpec& dir, bool onServer )
	{
		return FSTreeFromFSSpec( MacIO::FSMakeFSSpec< FNF_Throws >( dir, NULL ), onServer );
	}
	
	
	static const FSTreePtr& MakeFSRoot()
	{
		FSTree_Root* tree = NULL;
		
		static FSTreePtr result = FSTreePtr( tree = new FSTree_Root() );
		
		if ( tree != NULL )
		{
			tree->Map( FSTreeFromFSDirSpec( GetUsersDirectory(), false ) );
			
			tree->Map( FSTreePtr( new FSTree_Volumes( result, "Volumes" ) ) );
			tree->Map(            New_FSTree_proc   ( result, "proc"    )   );
			
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
		
		const bool exists = FSpGetCatInfo< FNF_Returns >( paramBlock, itIsOnServer, itsFileSpec );
		
		if ( exists )
		{
			const HFileInfo& hFileInfo = paramBlock.hFileInfo;
			
			const bool isDir = hFileInfo.ioFlAttrib & kioFlAttribDirMask;
			
			if ( isDir )
			{
				const N::FSDirSpec& root = GetJDirectory();
				
				return hFileInfo.ioVRefNum == root.vRefNum  &&  hFileInfo.ioDirID == root.dirID;
			}
			
			const bool isAlias = hFileInfo.ioFlFndrInfo.fdFlags & kIsAlias;
			
			return isAlias;
		}
		
		return false;
	}
	
	FSTreePtr FSTree_DirSpec::Parent() const
	{
		if ( itsDirSpec.dirID == fsRtDirID )
		{
			return Get_sys_mac_vol_N( itsDirSpec.vRefNum );
		}
		
		FSSpec fileSpec = FSMakeFSSpec< FNF_Throws >( itsDirSpec, NULL );
		
		const N::FSDirSpec& root = GetJDirectory();
		
		if ( fileSpec.vRefNum == root.vRefNum  &&  fileSpec.parID == root.dirID )
		{
			return FSRoot();
		}
		
		if ( itsDirSpec == GetUsersDirectory() )
		{
			return FSRoot();
		}
		
		return FSTreePtr( FSTreeFromFSDirSpec( io::get_preceding_directory( fileSpec ),
		                                       itIsOnServer ) );
	}
	
	FSTreePtr FSTree_HFS::Parent() const
	{
		if ( itsFileSpec.parID == fsRtParID )
		{
			return Get_sys_mac_vol_N( N::FSVolumeRefNum( itsFileSpec.vRefNum ) );
		}
		
		const N::FSDirSpec& root = GetJDirectory();
		
		if ( itsFileSpec.vRefNum == root.vRefNum  &&  itsFileSpec.parID == root.dirID )
		{
			return FSRoot();
		}
		
		if ( itsFileSpec == GetUsersDirectory() / "" )
		{
			return FSRoot();
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
	
	void FSTree_DirSpec::Stat( struct ::stat& sb ) const
	{
		Stat_HFS( false, &sb, itsDirSpec.vRefNum, itsDirSpec.dirID );
		
		const N::FSDirSpec& root = GetJDirectory();
		
		if ( itsDirSpec.vRefNum == root.vRefNum  &&  sb.st_ino == root.dirID )
		{
			sb.st_mode += S_IFLNK - S_IFDIR;
		}
	}
	
	void FSTree_HFS::Stat( struct ::stat& sb ) const
	{
		StatFile( itIsOnServer, GetFSSpec(), &sb, false );
		
		const N::FSDirSpec& root = GetJDirectory();
		
		if ( itsFileSpec.vRefNum == root.vRefNum  &&  sb.st_ino == root.dirID )
		{
			sb.st_mode += S_IFLNK - S_IFDIR;
		}
	}
	
	void FSTree_HFS::ChangeMode( mode_t mode ) const
	{
		ChangeFileMode( GetFSSpec(), mode );
	}
	
	void FSTree_DirSpec::SetTimes() const
	{
		SetFileTimes( itsDirSpec.vRefNum,
		              itsDirSpec.dirID,
		              NULL );
	}
	
	void FSTree_HFS::SetTimes() const
	{
		SetFileTimes( N::FSVolumeRefNum( itsFileSpec.vRefNum ),
		              N::FSDirID       ( itsFileSpec.parID   ),
		              itsFileSpec.name );
	}
	
	void FSTree_DirSpec::SetTimes( const struct timespec times[2] ) const
	{
		SetFileTimes( itsDirSpec.vRefNum,
		              itsDirSpec.dirID,
		              NULL,
		              times );
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
		
		// Unfortunately, fBsyErr can mean different things.
		// Here we assume it means a directory is not empty.
		
		if ( unlockErr == fnfErr  &&  deleteErr == fBsyErr )
		{
			p7::throw_errno( ENOTEMPTY );
		}
		
		N::ThrowOSStatus( deleteErr );
	}
	
	void FSTree_DirSpec::Delete() const
	{
		FSSpec fileSpec = FSMakeFSSpec< FNF_Throws >( itsDirSpec, NULL );
		
		Delete_HFS( fileSpec );
	}
	
	void FSTree_HFS::Delete() const
	{
		Delete_HFS( itsFileSpec );
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
		CInfoPBRec cInfo = { 0 };
		
		MacIO::GetCatInfo< FNF_Throws >( cInfo, file );
		
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
		
		try
		{
			file->Stat( stat_buffer );
		}
		catch ( const N::OSStatus& err )
		{
			if ( err != fnfErr )
			{
				throw;
			}
		}
		
		if ( stat_buffer.st_dev <= 0 )
		{
			// Not FSSpecified
			p7::throw_errno( EXDEV );
		}
		
		N::FSDirSpec parent;
		
		parent.vRefNum = N::FSVolumeRefNum( -stat_buffer.st_dev );
		parent.dirID   = N::FSDirID       ( stat_buffer.st_rdev );
		
		return parent / K::MacFilenameFromUnixFilename( file->Name() );
	}
	
	static void Rename_HFS( const FSSpec& srcFileSpec, const FSTreePtr& destFile )
	{
		if ( !io::item_exists( srcFileSpec ) )
		{
			p7::throw_errno( ENOENT );
		}
		
		bool destExists = destFile->Exists();
		
		bool srcIsDir  = io::directory_exists( srcFileSpec );
		bool destIsDir = destFile->IsDirectory();
		
		if ( destExists  &&  srcIsDir != destIsDir )
		{
			p7::throw_errno( destIsDir ? EISDIR : ENOTDIR );
		}
		
		FSSpec destFileSpec = GetFSSpecFromFSTree( destFile );
		
		// Can't move across volumes
		if ( srcFileSpec.vRefNum != destFileSpec.vRefNum )
		{
			p7::throw_errno( EXDEV );
		}
		
		N::FSVolumeRefNum vRefNum = N::FSVolumeRefNum( srcFileSpec.vRefNum );
		
		const std::string& destName = destFile->Name();
		
		const bool keeping_name =    destName.length() == srcFileSpec.name[0]
		                          && std::equal( destName.begin(),
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
	
	void FSTree_DirSpec::Rename( const FSTreePtr& destFile ) const
	{
		FSSpec srcFileSpec = FSMakeFSSpec< FNF_Throws >( itsDirSpec, NULL );
		
		Rename_HFS( srcFileSpec, destFile );
	}
	
	void FSTree_HFS::Rename( const FSTreePtr& destFile ) const
	{
		Rename_HFS( itsFileSpec, destFile );
	}
	
	
	off_t FSTree_HFS::GetEOF() const
	{
		CInfoPBRec paramBlock;
		
		FSpGetCatInfo< FNF_Throws >( paramBlock, itIsOnServer, GetFSSpec() );
		
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
		CInfoPBRec cInfo = { 0 };
		
		const bool exists = FSpGetCatInfo< FNF_Returns >( cInfo, itIsOnServer, GetFSSpec() );
		
		if ( !exists )
		{
			//return Self();
		}
		else if ( const bool is_dir = cInfo.hFileInfo.ioFlAttrib & kioFlAttribDirMask )
		{
			const N::FSDirSpec& root = GetJDirectory();
			
			const DirInfo& dirInfo = cInfo.dirInfo;
			
			if ( dirInfo.ioVRefNum == root.vRefNum  &&  dirInfo.ioDrDirID == root.dirID )
			{
				return FSRoot();
			}
		}
		else if ( cInfo.hFileInfo.ioFlFndrInfo.fdFlags & kIsAlias )
		{
			FSSpec target = N::ResolveAliasFile( GetFSSpec(), false );
			
			return FSTreeFromFSSpec( target, FileIsOnServer( target ) );
		}
		
		return Self();
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
		FSTreePtr target = ResolvePathname( targetPath, FSTreeFromFSDirSpec( linkParent, FileIsOnServer( linkSpec ) ) );
		
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
		CreateSymLink( Self(), target );
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
			CInfoPBRec cInfo = { 0 };
			
			N::Str255 actualName = itsFileSpec.name;
			
			const bool exists = FSpGetCatInfo< FNF_Returns >( cInfo,
			                                                  itIsOnServer,
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
		
		if ( created )
		{
			ChangeMode( mode );
		}
		
		return opened;
	}
	
	boost::shared_ptr< IOHandle > FSTree_HFS::Open( OpenFlags flags ) const
	{
		flags |= itIsOnServer ? O_MAC_ASYNC : 0;
		
		return OpenMacFileHandle( GetFSSpec(),
		                          flags,
		                          itIsOnServer ? &Genie::FSpOpenDF : N::FSpOpenDF,
		                          &New_DataForkHandle );
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
	
	
	static FSTreePtr FSTreePtr_From_Lookup( const N::FSDirSpec& dir, bool onServer, const std::string& name )
	{
		const std::string macName = K::MacFilenameFromUnixFilename( name );
		
		const FSSpec item = dir / macName;
		
		return FSTreePtr( new FSTree_HFS( item, onServer, name ) );
	}
	
	FSTreePtr FSTree_Root::Lookup_Regular( const std::string& name ) const
	{
		return FSTreePtr_From_Lookup( GetJDirectory(), false, name );
	}
	
	FSTreePtr FSTree_DirSpec::Lookup_Child( const std::string& name ) const
	{
		return FSTreePtr_From_Lookup( itsDirSpec, itIsOnServer, name );
	}
	
	FSTreePtr FSTree_HFS::Lookup_Child( const std::string& name ) const
	{
		if ( name == "rsrc"  &&  IsFile() )
		{
			return GetRsrcForkFSTree( itsFileSpec, itIsOnServer );
		}
		
		if ( name == "res"  &&  IsFile() )
		{
			return Get_ResFileDir_FSTree( itsFileSpec, itIsOnServer );
		}
		
		N::FSDirSpec dir = Dir_From_FSSpec( itsFileSpec );
		
		return FSTreePtr_From_Lookup( dir, itIsOnServer, name );
	}
	
	
	const UInt16 kMaxItems = 4;
	
	struct NameAndID
	{
		N::Str31    name;
		N::FSDirID  id;
	};
	
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
		
		const bool async = !TARGET_CPU_68K && FileIsOnServer( item ) && !RunningInClassic::Test();
		
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
			
			cache.reserve( cache.size() + pb.n_items );
			
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
			
			N::ThrowOSStatus( pb.dirInfo.ioResult );
		}
	}
	
	void FSTree_Root::IterateIntoCache( FSTreeCache& cache ) const
	{
		IterateIntoCache_CInfoPBRec cInfo;
		
		const N::FSDirSpec& root = GetJDirectory();
		
		cInfo.dirInfo.ioNamePtr   = NULL;
		cInfo.dirInfo.ioVRefNum   = root.vRefNum;
		cInfo.dirInfo.ioDrDirID   = root.dirID;
		cInfo.dirInfo.ioFDirIndex = -1;
		
		IterateFilesIntoCache( cInfo, cache );
	}
	
	void FSTree_DirSpec::IterateIntoCache( FSTreeCache& cache ) const
	{
		IterateIntoCache_CInfoPBRec cInfo;
		
		FSpGetCatInfo< FNF_Throws >( cInfo, itIsOnServer, itsDirSpec );
		
		IterateFilesIntoCache( cInfo, cache );
	}
	
	void FSTree_HFS::IterateIntoCache( FSTreeCache& cache ) const
	{
		IterateIntoCache_CInfoPBRec cInfo;
		
		FSpGetCatInfo< FNF_Throws >( cInfo, itIsOnServer, GetFSSpec() );
		
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
			N::ThrowOSStatus( dirInfo.ioResult );
		}
		
		const SInt32 dirID = nonexistent ? dirInfo.ioDrDirID : dirInfo.ioDrParID;
		
		FSSpec result = { dirInfo.ioVRefNum, dirID };
		
		N::CopyToPascalString( cInfo.name, result.name, sizeof result.name - 1 );
		
		// This code path is only used on servers.
		
		return FSTreeFromFSSpec( result, true );
	}
	
	FSTreePtr FSTree_HFS::ResolvePath( const char*& begin, const char* end ) const
	{
		if ( begin == end )
		{
			return Self();
		}
		
		ASSERT( begin < end );
		
		if (     TARGET_CPU_68K
		     ||  !itIsOnServer
		     ||  name_is_special( begin, std::find( begin, end, '/' ) )
		     ||  RunningInClassic::Test() )
		{
			// Special handling required for
			// * dot, dotdot, colons, and long names
			// * running in Classic, which has broken PBGetCatInfoAsync()
			
			return FSTree::ResolvePath( begin, end );
		}
		
		return ResolvePath_HFS( itsFileSpec, begin, end );
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
			
			FSTreePtr ResolveLink() const
			{
				return FSTreeFromFSSpec( FSSpecFromKey( itsKey ), VolumeIsOnServer( itsKey ) );
			}
	};
	
	
	FSTreePtr Volumes_Details::GetChildNode( const FSTreePtr&    parent,
		                                     const std::string&  name,
		                                     const Key&          key ) const
	{
		return FSTreePtr( new FSTree_Volumes_Link( parent, name, key ) );
	}
	
}

