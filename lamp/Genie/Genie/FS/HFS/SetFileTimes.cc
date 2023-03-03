/*
	Genie/FS/HFS/SetFileTimes.hh
	----------------------------
*/

#include "Genie/FS/HFS/SetFileTimes.hh"

// POSIX
#include <utime.h>

// mac-types
#include "mac_types/epoch.hh"

// Nitrogen
#include "Nitrogen/Files.hh"

// MacIO
#include "MacIO/GetCatInfo_Sync.hh"


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
				GetDateTime( &now );
			}
			
			date = now;
		}
		else
		{
			date = time.tv_sec + mac::types::epoch_delta();
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
