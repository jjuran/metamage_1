/*	===============
 *	FSWrite_Sync.hh
 *	===============
 */

#ifndef MACIO_FSWRITESYNC_HH
#define MACIO_FSWRITESYNC_HH

// Nitrogen
#include "Mac/Files/Types/FSFileRefNum.hh"
#include "Mac/Files/Types/FSIOPosMode.hh"


namespace MacIO
{
	
	SInt32 FSWrite( Mac::FSFileRefNum  file,
	                Mac::FSIOPosMode   positionMode,
	                SInt32             positionOffset,
	                SInt32             requestCount,
	                const void *       buffer );
	
}

#endif

