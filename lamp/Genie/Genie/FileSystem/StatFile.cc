/*	===========
 *	StatFile.cc
 *	===========
 */

#include "Genie/FileSystem/StatFile.hh"

// Standard C++
#include <string>

// POSIX
#include "stdlib.h"
#include "sys/stat.h"

// MoreFunctional
#include "PointerToFunction.hh"

// POSeven
#include "POSeven/Errno.hh"

// Nitrogen
#include "Nitrogen/Files.h"

// TimeOff
#include "TimeOff.hh"

// Genie
#include "Genie/FileSignature.hh"
#include "Genie/Process/AsyncYield.hh"


namespace Nitrogen
{
	
	static const Gestalt_Selector gestaltMacOSCompatibilityBoxAttr = Gestalt_Selector( ::gestaltMacOSCompatibilityBoxAttr );
	
	template <> struct GestaltDefault< gestaltMacOSCompatibilityBoxAttr > : GestaltAttrDefaults {};
	
}

namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	static bool RunningInClassic()
	{
		static bool inClassic = N::Gestalt_Bit< N::gestaltMacOSCompatibilityBoxAttr, gestaltMacOSCompatibilityBoxPresent >();
		
		return inClassic;
	}
	
	
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
	
	void StatFile( const FSSpec& file, struct stat* sb, bool wantRsrcFork )
	{
		const unsigned long timeDiff = TimeOff::MacToUnixTimeDifference();
		
		CInfoPBRec paramBlock = { 0 };
		
		// Calling the asynchronous variant of FSpGetCatInfo() reliably elicits
		// System Error 27 (dsFSErr: file system map has been trashed) in Classic
		// (eventually), but (so far) only when called from CheckProgramFile().
		
		// On infrequent (but not rare) occasion, ioResult is set and control is
		// returned to the application (allowing it to destroy the parameter block)
		// before the completion routine has run.  Sadly, polling a flag set by
		// the completion routine only delays the crash instead of avoiding it.
		// Apparently this is a bug in the .BlueBoxShared driver.
		
		// We could set have CheckProgramFile() set an indicator in one of sb's
		// fields, but (a) the same bug may affect other uses besides, even if
		// they haven't shown up yet, and (b) it's just not worth it to hack
		// around Apple's bug like that.
		
		if ( RunningInClassic() )
		{
			N::FSpGetCatInfo( file, paramBlock );
		}
		else
		{
			N::FSpGetCatInfo( file, paramBlock, more::ptr_fun( AsyncYield ) );
		}
		
		const HFileInfo& hFileInfo = paramBlock.hFileInfo;
		
		bool isDir = hFileInfo.ioFlAttrib & kioFlAttribDirMask;
		
		sb->st_dev = -hFileInfo.ioVRefNum;  // inverted vRefNum (positive integer) for device
		sb->st_ino = hFileInfo.ioDirID;     // file or dir ID for inode
		sb->st_mode = GetItemMode( hFileInfo );
		// dirs: # of items (including . and ..)
		// files: # of hard links (always one, for now)
		sb->st_nlink = isDir ? paramBlock.dirInfo.ioDrNmFls + 2: 1;
		sb->st_uid = 0;
		sb->st_gid = 0;
		sb->st_rdev = 0;
		// logical fork length in bytes
		sb->st_size = isDir ? 0
		                    : wantRsrcFork ? hFileInfo.ioFlRLgLen
		                                   : hFileInfo.ioFlLgLen;
		// preferred I/O blocking factor for buffering
		sb->st_blksize = 4096;
		// physical fork length in 512-byte blocks
		sb->st_blocks = isDir ? 0
		                      : (wantRsrcFork ? hFileInfo.ioFlRPyLen
		                                      : hFileInfo.ioFlPyLen) / 512;
		// time of last access:  pretend mod time; provide backup stamp for rsrc.
		sb->st_atime = (wantRsrcFork ? hFileInfo.ioFlBkDat : hFileInfo.ioFlMdDat) - timeDiff;
		// time of last modification.
		sb->st_mtime =                                       hFileInfo.ioFlMdDat  - timeDiff;
		// time of last inode change:  pretend mod time; provide creation stamp for rsrc.
		sb->st_ctime = (wantRsrcFork ? hFileInfo.ioFlCrDat : hFileInfo.ioFlMdDat) - timeDiff;
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
		
		N::FSpGetCatInfo( file, paramBlock, more::ptr_fun( AsyncYield ) );
		
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

