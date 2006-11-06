/*	===========
 *	StatFile.cc
 *	===========
 */

#include "Genie/FileSystem/StatFile.hh"

// Universal Interfaces
#include <OSUtils.h>

// Standard C++
#include <string>

// POSIX
#include "stdlib.h"
#include "sys/stat.h"

// POSeven
#include "POSeven/Errno.hh"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"

// Genie
#include "Genie/FileSignature.hh"


namespace Nitrogen
{
	
	static void FSpSetFInfo( const FSSpec& file, const FInfo& info )
	{
		ThrowOSStatus( ::FSpSetFInfo( &file, &info ) );
	}
	
	static void FSpSetFLock( const FSSpec& file )
	{
		ThrowOSStatus( ::FSpSetFLock( &file ) );
	}
	
	static void FSpRstFLock( const FSSpec& file )
	{
		ThrowOSStatus( ::FSpRstFLock( &file ) );
	}
	
}

namespace Genie
{
	
	namespace N = Nitrogen;
	namespace P7 = POSeven;
	
	
	inline unsigned long MacUnixEpochOffset()
	{
		// Returns the number of seconds between Mac and Unix epochs (global time).
		// Mac time - Unix time = offset
		// Mac time - offset = Unix time
		// Unix time + offset = Mac time
		
		enum { macYear = 1904, unixYear = 1970 };
		
		const unsigned years = unixYear - macYear;  // 66
		const unsigned quads = years / 4;  // 16
		const unsigned extra = years % 4;  // 2
		const unsigned daysPerQuad = 4 * 365 + 1;  // One Feb 29 per four years
		const unsigned extraDays = extra * 365 + 1;  // First year is a leap year
		const unsigned daysOffset = daysPerQuad * quads + extraDays;
		const unsigned secondsPerDay = 60 * 60 * 24;
		const unsigned long kMacUnixEpochOffset = daysOffset * secondsPerDay;
		
		return kMacUnixEpochOffset;
	}
	
	static long MacLocalTimeDeltaForFiles( long gmtDelta )
	{
		// Returns the delta in seconds between global time and File Manager time.
		// Call it with loc.u.gmtDelta (without masking the high byte) after calling
		// ReadLocation( &loc ).
		// Subtract the result from a File Manager date to get a GMT date (Mac epoch).
		// Add it to a GMT date to get a File Manager date.
		
		// 7th bit of high byte indicates Daylight Savings Time is in effect
		bool dls = gmtDelta & 0x80000000;
		
		// Mask off DLS byte
		gmtDelta &= 0x00FFFFFF;
		
		// gmtDelta is the number of seconds ahead of GMT we are.
		// For EST, it's -5 * 3600 = -18000.  For EDT, it's -4 * 3600 = -14400.
		
		// If delta is negative we need to sign-extend it
		bool signExtend = gmtDelta & 0x00800000;
		
		// Sign-extend if required
		gmtDelta |= signExtend ? 0xFF000000 : 0x00000000;
		
		// The Mac OS File Manager returns dates in local standard time (e.g. EST)
		// regardless of Daylight Savings Time.  So if DLS is in effect, we have to
		// compensate.
		
		// If DLS is in effect (clocks set forward), subtract an hour
		gmtDelta -= dls ? 3600 : 0;
		
		return gmtDelta;
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
		
		mode_t mode = S_IRUSR | ( isDir ? S_IFDIR | S_IWUSR | S_IXUSR
		                                : S_IFREG | FileWXModeBits( hFileInfo ) );
		
		return mode;
	}
	
	void StatFile( const FSSpec& file, struct stat* sb, bool wantRsrcFork )
	{
		MachineLocation loc;
		ReadLocation( &loc );
		
		const unsigned long timeDiff = MacUnixEpochOffset() + MacLocalTimeDeltaForFiles( loc.u.gmtDelta );
		
		CInfoPBRec paramBlock;
		
		N::FSpGetCatInfo( file, paramBlock );
		
		const HFileInfo& hFileInfo = paramBlock.hFileInfo;
		
		bool isDir = hFileInfo.ioFlAttrib & kioFlAttribDirMask;
		
		sb->st_dev = -hFileInfo.ioVRefNum;
		sb->st_ino = hFileInfo.ioDirID;
		sb->st_mode = GetItemMode( hFileInfo );
		sb->st_nlink = isDir ? paramBlock.dirInfo.ioDrNmFls + 2: 1;
		sb->st_uid = 0;
		sb->st_gid = 0;
		sb->st_rdev = 0;
		sb->st_size = isDir ? 0
		                    : wantRsrcFork ? hFileInfo.ioFlRLgLen
		                                   : hFileInfo.ioFlLgLen;
		sb->st_blksize = 4096;
		sb->st_blocks = isDir ? 0
		                      : (wantRsrcFork ? hFileInfo.ioFlRPyLen
		                                      : hFileInfo.ioFlPyLen) / 512;
		sb->st_atime = hFileInfo.ioFlMdDat - timeDiff;
		sb->st_mtime = hFileInfo.ioFlMdDat - timeDiff;
		sb->st_ctime = hFileInfo.ioFlMdDat - timeDiff;
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
		CInfoPBRec paramBlock;
		
		N::FSpGetCatInfo( file, paramBlock );
		
		HFileInfo& hFileInfo = paramBlock.hFileInfo;
		
		bool isDir = hFileInfo.ioFlAttrib & kioFlAttribDirMask;
		
		if ( isDir )
		{
			// Can't change permissions on directories
			P7::ThrowErrno( EPERM );
		}
		
		mode_t current_mode = GetItemMode( hFileInfo );
		
		mode_t changed_bits = new_mode ^ current_mode;
		
		if ( changed_bits & S_IRUSR )
		{
			// Can't make anything unreadable
			throw P7::Errno( EPERM );
		}
		
		if ( changed_bits & S_IXUSR )
		{
			FInfo& info = hFileInfo.ioFlFndrInfo;
			
			if ( info.fdType != 'TEXT' )
			{
				// Can't change executability of non-scripts
				// (e.g. don't remove Shared bit on apps)
				throw P7::Errno( EPERM );
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

