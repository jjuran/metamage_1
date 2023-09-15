/*
	Genie/FS/HFS/SetFileTimes.hh
	----------------------------
*/

#ifndef GENIE_FS_HFS_SETFILETIMES_HH
#define GENIE_FS_HFS_SETFILETIMES_HH

// Standard C
#include <time.h>


namespace Genie
{
	
	void SetFileTimes( short                  vRefNum,
	                   long                   dirID,
	                   const unsigned char*   name,
	                   const struct timespec  times[2] );
	
}

#endif
