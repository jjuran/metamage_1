/*
	Genie/FS/HFS/SetFileTimes.hh
	----------------------------
*/

#include "Genie/FS/HFS/SetFileTimes.hh"

// POSIX
#include <utime.h>

// Nitrogen
#include "Nitrogen/DateTimeUtils.hh"
#include "Nitrogen/Files.hh"

// MacIO
#include "MacIO/GetCatInfo_Sync.hh"

// TimeOff
#include "TimeOff/TimeOff.hh"


#ifndef UTIME_ARCHIVE
#define UTIME_ARCHIVE  0x40000000
#endif

#ifndef UTIME_NOW
#define UTIME_NOW   ((1 << 30) - 1)
#endif

#ifndef UTIME_OMIT
#define UTIME_OMIT  ((1 << 30) - 2)
#endif


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	void SetFileTimes( Mac::FSVolumeRefNum   vRefNum,
	                   Mac::FSDirID          dirID,
	                   const unsigned char*  name )
	{
		UInt32 modTime = N::GetDateTime();
		
		N::Str63 name_copy = name;
		
		CInfoPBRec paramBlock;
		
		MacIO::GetCatInfo< MacIO::Throw_All >( paramBlock, vRefNum, dirID, name_copy );
		
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
	
	void SetFileTimes( Mac::FSVolumeRefNum    vRefNum,
	                   Mac::FSDirID           dirID,
	                   const unsigned char*   name,
	                   const struct timespec  times[2] )
	{
		const timespec& atime = times[0];
		const timespec& mtime = times[1];
		
		const bool atime_is_backup = atime.tv_nsec & UTIME_ARCHIVE;
		
		N::Str63 name_copy = name;
		
		CInfoPBRec paramBlock;
		
		MacIO::GetCatInfo< MacIO::Throw_All >( paramBlock, vRefNum, dirID, name_copy );
		
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
	
}

