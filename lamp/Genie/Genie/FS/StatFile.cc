/*	===========
 *	StatFile.cc
 *	===========
 */

#include "Genie/FS/StatFile.hh"

// Standard C/C++
#include <cstring>

// POSIX
#include "stdlib.h"
#include "sys/stat.h"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/Files.hh"

// MacIO
#include "MacIO/GetCatInfo_Sync.hh"

// TimeOff
#include "TimeOff/TimeOff.hh"

// Genie
#include "Genie/FileSignature.hh"
#include "Genie/Utilities/AsyncIO.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	static bool TypeIsExecutable( OSType type )
	{
		switch ( type )
		{
			case 'Wish':
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
		
		return ( writable ? S_IWUSR : 0 ) | ( executable ? S_IXUSR : 0 );
	}
	
	static mode_t GetItemMode( const HFileInfo& hFileInfo )
	{
		bool isDir = hFileInfo.ioFlAttrib & kioFlAttribDirMask;
		
		bool isAlias = !isDir  &&  hFileInfo.ioFlFndrInfo.fdFlags & kIsAlias;
		
		mode_t mode = isDir   ? S_IFDIR | S_IRUSR | S_IWUSR | S_IXUSR
		            : isAlias ? S_IFLNK | S_IRUSR | S_IWUSR | S_IXUSR
		            :           S_IFREG | S_IRUSR | FileWXModeBits( hFileInfo );
		
		return mode;
	}
	
	void Stat_HFS( bool                      async,
	               struct ::stat*            sb,
	               Nitrogen::FSVolumeRefNum  vRefNum,
	               Nitrogen::FSDirID         dirID,
	               const unsigned char*      name,
	               bool                      is_rsrc_fork )
	{
		const unsigned long timeDiff = TimeOff::MacToUnixTimeDifference();
		
		N::Str255 name_copy = name != NULL ? name : "\p";
		
		CInfoPBRec cInfo = { 0 };
		
		const bool exists = FSpGetCatInfo< FNF_Returns >( cInfo, async, vRefNum, dirID, name_copy );
		
		const HFileInfo& hFileInfo = cInfo.hFileInfo;
		
		sb->st_dev = -hFileInfo.ioVRefNum;  // inverted vRefNum (positive integer) for device
		sb->st_rdev = hFileInfo.ioFlParID;  // dir ID of parent
		
		if ( name_copy[0] > 31 )
		{
			throw N::StringTooLong();
		}
		
	#ifdef __LAMP__
		
		std::memcpy( sb->st_name, name_copy, 1 + name_copy[0] );
		
	#endif
		
		if ( !exists )
		{
			// Treating this specially (a) avoids throw_errno_internal(), which
			// would make life hell if we had set a breakpoint there, and (b)
			// lets us pass partial results back before throwing.
			
			sb->st_rdev = dirID;
			
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
		
	#ifdef __LAMP__
		
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
	
	void ChangeFileMode( const FSSpec& file, mode_t new_mode )
	{
		CInfoPBRec paramBlock = { 0 };
		
		MacIO::GetCatInfo< FNF_Throws >( paramBlock, file );
		
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
			
			if ( info.fdType != 'TEXT' )
			{
				// Can't change executability of non-scripts
				// (e.g. don't remove Shared bit on apps)
				p7::throw_errno( EPERM );
			}
			
			bool x_bit = new_mode & S_IXUSR;
			
			info.fdFlags = (info.fdFlags & ~kIsShared) | (kIsShared * x_bit);
			info.fdCreator = x_bit ? 'Poof' : TextFileCreator();
			
			N::FSpSetFInfo( file, info );
		}
		
		if ( changed_bits & S_IWUSR )
		{
			if ( new_mode & S_IWUSR )
			{
				// writeable: reset the lock
				N::FSpRstFLock( file );
			}
			else
			{
				// not writeable: set the lock
				N::FSpSetFLock( file );
			}
		}
	}
	
}

