/*	=======
 *	stat.cc
 *	=======
 */

// Universal Interfaces
#include <OSUtils.h>

// Standard C
#include <errno.h>
#include <string.h>

// Standard C++
#include <string>

// POSIX
#include "stdlib.h"
#include "sys/stat.h"

// Nitrogen / Carbon
#include "Nitrogen/OSStatus.h"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"

// Kerosene/Common
#include "KEnvironment.hh"
#include "SystemCalls.hh"

// Genie
#include "Genie/FileHandle.hh"
#include "Genie/pathnames.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
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
	
	static void StatFile( const FSSpec& file, struct stat* sb )
	{
		MachineLocation loc;
		ReadLocation( &loc );
		
		const unsigned long timeDiff = MacUnixEpochOffset() + MacLocalTimeDeltaForFiles( loc.u.gmtDelta );
		
		CInfoPBRec paramBlock;
		
		N::FSpGetCatInfo( file, paramBlock );
		
		bool isDir = N::PBTestIsDirectory( paramBlock );
		
		mode_t mode = isDir ? S_IFDIR | S_IXUSR : S_IFREG;
		mode |= S_IRUSR;
		
		sb->st_dev = 0;
		sb->st_ino = paramBlock.hFileInfo.ioDirID;
		sb->st_mode = mode;
		sb->st_nlink = 1;
		sb->st_uid = 0;
		sb->st_gid = 0;
		sb->st_rdev = 0;
		sb->st_size = paramBlock.hFileInfo.ioFlLgLen;
		sb->st_blksize = 4096;
		sb->st_blocks = paramBlock.hFileInfo.ioFlPyLen / 512;
		sb->st_atime = paramBlock.hFileInfo.ioFlMdDat - timeDiff;
		sb->st_mtime = paramBlock.hFileInfo.ioFlMdDat - timeDiff;
		sb->st_ctime = paramBlock.hFileInfo.ioFlMdDat - timeDiff;
	}
	
	static void StatGeneric( struct stat* sb )
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
	
	static void StatCharacterDevice( struct stat* sb )
	{
		StatGeneric( sb );
	}
	
	static int stat_file( const char* path, struct stat* sb )
	{
		std::memset( (void*)sb, '\0', sizeof (struct stat) );
		
		std::string pathname = path;
		
		IORef io;
		
		if ( pathname.substr( 0, 5 ) == "/dev/" )
		{
			
		}
		else
		{
			// assume it's a file
			
			try
			{
				N::FSDirSpec current = N::Convert< N::FSDirSpec >( CurrentProcess().CurrentDirectory() );
				
				FSSpec file = ResolveUnixPathname( path, current );
				
				StatFile( file, sb );
				
				return 0;
			}
			catch ( N::FNFErr& )
			{
				return CurrentProcess().SetErrno( ENOENT );
			}
			catch ( N::OSStatus& err )
			{
				
			}
		}
		
		return CurrentProcess().SetErrno( EINVAL );
	}
	
	static int lstat( const char* path, struct stat* sb )
	{
		return stat_file( path, sb );
	}
	
	REGISTER_SYSTEM_CALL( lstat );
	
	static int stat( const char* path, struct stat* sb )
	{
		return stat_file( path, sb );  // FIXME:  Resolve symlinks
	}
	
	REGISTER_SYSTEM_CALL( stat );
	
	static FSSpec FSSpecFromFRefNum( N::FSFileRefNum refNum )
	{
		FSSpec result;
		
		FCBPBRec pb;
		
		pb.ioVRefNum = 0;
		pb.ioFCBIndx = 0;
		pb.ioRefNum  = refNum;
		pb.ioNamePtr = result.name;
		
		N::ThrowOSStatus( ::PBGetFCBInfoSync( &pb ) );
		
		result.vRefNum = pb.ioFCBVRefNum;
		result.parID = pb.ioFCBParID;
		
		return result;
	}
	
	static int fstat( int fd, struct stat* sb )
	{
		Genie::FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		try
		{
			using Genie::kFileDescriptor;
			using Genie::kGenericIODescriptor;
			using Genie::kCharacterDeviceDescriptor;
			
			Genie::IORef handle = files[ fd ].handle;
			
			// FIXME:  Verify file exists first
			if ( handle.IsType( kFileDescriptor ) )
			{
				using Genie::FileHandle;
				using Genie::IORef_Cast;
				
				const FileHandle& fh = IORef_Cast< FileHandle >( handle );
				
				const N::FSFileRefNum refNum = fh.GetRefNum();
				
				FSSpec file = FSSpecFromFRefNum( refNum );
				
				StatFile( file, sb );
				
				return 0;
			}
			else if ( handle.IsType( kCharacterDeviceDescriptor ) )
			{
				StatCharacterDevice( sb );
			}
			else if ( handle.IsType( kGenericIODescriptor ) )
			{
				StatGeneric( sb );
			}
			else
			{
				return CurrentProcess().SetErrno( EBADF );
			}
		}
		catch ( ... )
		{
			// Convert the exception
			return CurrentProcess().SetErrno( EINVAL );
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( fstat );
	
}

