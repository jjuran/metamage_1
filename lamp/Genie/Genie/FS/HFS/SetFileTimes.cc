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
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"


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
		OSErr err;
		
		const timespec& atime = times[0];
		const timespec& mtime = times[1];
		
		const bool atime_is_backup = atime.tv_nsec & UTIME_ARCHIVE;
		
		CInfoPBRec pb;
		
		DirInfo& dirInfo = pb.dirInfo;
		
		dirInfo.ioNamePtr = (StringPtr) name;
		dirInfo.ioVRefNum = vRefNum;
		dirInfo.ioDrDirID = dirID;
		dirInfo.ioFDirIndex = 0;
		
		err = PBGetCatInfoSync( &pb );
		
		if ( err )
		{
			goto errorcheck;
		}
		
		pb.hFileInfo.ioDirID = dirID;
		
		UInt32 now;
		
		now = 0;
		
		//pb.hFileInfo.ioFlCrDat
		
		update_time( pb.hFileInfo.ioFlMdDat, mtime, now );
		
		if ( atime_is_backup )
		{
			update_time( pb.hFileInfo.ioFlBkDat, atime, now );
		}
		
		err = PBSetCatInfoSync( &pb );
		
	errorcheck:
		
		Mac::ThrowOSStatus( err );
	}
	
}
