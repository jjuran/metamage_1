/*	===========
 *	StatFile.cc
 *	===========
 */

#include "Genie/FS/StatFile.hh"

// POSIX
#include "stdlib.h"
#include "sys/stat.h"

// more-libc
#include "more/string.h"

// mac-types
#include "mac_types/epoch.hh"

// mac-file-utils
#include "mac_file/open_data_fork.hh"
#include "mac_file/rw.hh"

// gear
#include "gear/is_binary_data.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/Files.hh"

// MacIO
#include "MacIO/GetCatInfo_Sync.hh"

// Genie
#include "Genie/FileSignature.hh"
#include "Genie/FS/FSTree_FSSpec.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	static bool TypeIsExecutable( OSType type )
	{
		switch ( type )
		{
			case 'Tool':
			case 'MPST':
			case 'APPL':
				return true;
			
			default:
				break;
		}
		
		return false;
	}
	
	static bool FInfoIsExecutable( const FInfo& info )
	{
		::OSType type = info.fdType;
		
		if ( TypeIsExecutable( type ) )  return true;
		
		if ( type != 'TEXT' )  return false;
		
		return info.fdFlags & kIsShared || info.fdCreator == 'Poof';
	}
	
	static mode_t FileWXModeBits( const HFileInfo& hFileInfo )
	{
		bool locked = hFileInfo.ioFlAttrib & kioFlAttribLockedMask;
		bool writable = !locked;
		
		const FInfo& info = hFileInfo.ioFlFndrInfo;
		bool executable = FInfoIsExecutable( info );
		
		return ( writable ? 0200 : 0 ) | ( executable ? 0111 : 0 );
	}
	
	static inline bool is_osx_symlink( const FInfo& fInfo )
	{
		return    fInfo.fdCreator == Mac::kSymLinkCreator
		       && fInfo.fdType    == Mac::kSymLinkFileType;
	}
	
	mode_t GetItemMode( const HFileInfo& hFileInfo )
	{
		if ( hFileInfo.ioResult < 0 )
		{
			return 0;
		}
		
		if ( const bool is_dir = hFileInfo.ioFlAttrib & kioFlAttribDirMask )
		{
			const mac::types::VRefNum_DirID& root = root_DirSpec();
			
			const bool is_root = hFileInfo.ioVRefNum == root.vRefNum  &&  hFileInfo.ioDirID == root.dirID;
			
			if ( is_root )
			{
				return S_IFLNK | 0777;
			}
			
			return S_IFDIR | 0755;
		}
		
		const FInfo& fInfo = hFileInfo.ioFlFndrInfo;
		
		const bool is_alias = fInfo.fdFlags & kIsAlias;
		
		const bool is_link = is_alias  ||  is_osx_symlink( fInfo );
		
		if ( is_link )
		{
			return S_IFLNK | 0777;
		}
		
		if ( fInfo.fdCreator == 'Poof' )
		{
			if ( fInfo.fdType == 'SOCK' )
			{
				return S_IFSOCK | 0600;
			}
			
			if ( (fInfo.fdType & 0xFFFFFF00) == 'FIF\0' )
			{
				return S_IFIFO | 0600;
			}
		}
		
		return S_IFREG | 0444 | FileWXModeBits( hFileInfo );
	}
	
	void Stat_HFS( struct stat*          sb,
	               const CInfoPBRec&     cInfo,
	               const unsigned char*  name,
	               bool                  is_rsrc_fork )
	{
		const unsigned long timeDiff = mac::types::epoch_delta();
		
		const HFileInfo& hFileInfo = cInfo.hFileInfo;
		
		const bool exists = hFileInfo.ioResult == noErr;
		
		sb->st_dev = -hFileInfo.ioVRefNum;  // inverted vRefNum (positive integer) for device
		sb->st_rdev = hFileInfo.ioFlParID;  // dir ID of parent
		
		if ( name[0] > 31 )
		{
			throw N::StringTooLong();
		}
		
	#ifdef __RELIX__
		
		mempcpy( sb->st_name, name, 1 + name[0] );
		
	#endif
		
		if ( !exists )
		{
			// Treating this specially (a) avoids throw_errno_internal(), which
			// would make life hell if we had set a breakpoint there, and (b)
			// lets us pass partial results back before throwing.
			
			sb->st_rdev = hFileInfo.ioDirID;
			
			throw p7::errno_t( ENOENT );
		}
		
		const bool is_dir = hFileInfo.ioFlAttrib & kioFlAttribDirMask;
		
		sb->st_ino = hFileInfo.ioDirID;     // file or dir ID for inode
		sb->st_mode = GetItemMode( hFileInfo );
		// dirs: # of items (including . and ..)
		// files: # of hard links (always one, for now)
		sb->st_nlink = is_dir ? cInfo.dirInfo.ioDrNmFls + 2: 1;
		sb->st_uid = 0;
		sb->st_gid = 0;
		// logical fork length in bytes
		sb->st_size = is_dir ? 0
		                     : is_rsrc_fork ? hFileInfo.ioFlRLgLen
		                                    : hFileInfo.ioFlLgLen;
		// preferred I/O blocking factor for buffering
		sb->st_blksize = 4096;
		// physical fork length in 512-byte blocks
		sb->st_blocks = is_dir ? 0
		                       : (is_rsrc_fork ? hFileInfo.ioFlRPyLen
		                                       : hFileInfo.ioFlPyLen) / 512;
		// time of last access:  pretend mod time; provide backup stamp for rsrc.
		sb->st_atime = (is_rsrc_fork ? hFileInfo.ioFlBkDat : hFileInfo.ioFlMdDat) - timeDiff;
		// time of last modification.
		sb->st_mtime =                                       hFileInfo.ioFlMdDat  - timeDiff;
		// time of last inode change:  pretend mod time; provide creation stamp for rsrc.
		sb->st_ctime = (is_rsrc_fork ? hFileInfo.ioFlCrDat : hFileInfo.ioFlMdDat) - timeDiff;
		
	#ifdef __RELIX__
		
		sb->st_birthtim.tv_sec = hFileInfo.ioFlCrDat - timeDiff;
		sb->st_checktim.tv_sec = hFileInfo.ioFlBkDat - timeDiff;
		
	#endif
	}
	
	
	void StatGeneric( struct stat* sb )
	{
		mode_t mode = S_IFCHR;
		
		sb->st_dev = 0;
		sb->st_ino = 0;
		sb->st_mode = mode;
		sb->st_nlink = 1;
		sb->st_uid = 0;
		sb->st_gid = 0;
		sb->st_rdev = 0;
		sb->st_size = 0;
		sb->st_blksize = 4096;
		sb->st_blocks = 0;
		sb->st_atime = 0;
		sb->st_mtime = 0;
		sb->st_ctime = 0;
		
	}
	
	static bool is_text_file( const FSSpec& file )
	{
		using mac::file::FSIORefNum;
		using mac::file::open_data_fork;
		
		const size_t buffer_size = TARGET_API_MAC_CARBON ? 4096 : 512;
		
		char buffer[ buffer_size ];
		
		bool result = false;
		
		FSIORefNum in = open_data_fork( file, fsRdPerm );
		
		if ( in >= 0 )
		{
			Size n_bytes = mac::file::read( in, buffer, buffer_size );
			
			if ( n_bytes >= 0 )
			{
				result = ! gear::is_binary_data( buffer, n_bytes );
			}
			
			FSClose( in );
		}
		
		return result;
	}
	
	void ChangeFileMode( const FSSpec& file, mode_t new_mode )
	{
		CInfoPBRec paramBlock;
		
		MacIO::GetCatInfo< MacIO::Throw_All >( paramBlock, file );
		
		HFileInfo& hFileInfo = paramBlock.hFileInfo;
		
		bool isDir = hFileInfo.ioFlAttrib & kioFlAttribDirMask;
		
		if ( isDir )
		{
			// Can't change permissions on directories
			p7::throw_errno( EPERM );
		}
		
		mode_t current_mode = GetItemMode( hFileInfo );
		
		mode_t changed_bits = new_mode ^ current_mode;
		
		if ( changed_bits & S_IRUSR )
		{
			// Can't make anything unreadable
			p7::throw_errno( EPERM );
		}
		
		if ( changed_bits & S_IXUSR )
		{
			FInfo& info = hFileInfo.ioFlFndrInfo;
			
			if ( info.fdType == 0  &&  is_text_file( file ) )
			{
				info.fdType = 'TEXT';
			}
			else if ( info.fdType != 'TEXT' )
			{
				// Can't change executability of non-scripts
				// (e.g. don't remove Shared bit on apps)
				p7::throw_errno( EPERM );
			}
			
			bool x_bit = new_mode & S_IXUSR;
			
			info.fdFlags = (info.fdFlags & ~kIsShared) | (kIsShared * x_bit);
			info.fdCreator = x_bit ? 'Poof' : TextFileCreator();
			
			N::HSetFInfo( file, info );
		}
		
		if ( changed_bits & S_IWUSR )
		{
			if ( new_mode & S_IWUSR )
			{
				// writeable: reset the lock
				N::HRstFLock( file );
			}
			else
			{
				// not writeable: set the lock
				N::HSetFLock( file );
			}
		}
	}
	
}
