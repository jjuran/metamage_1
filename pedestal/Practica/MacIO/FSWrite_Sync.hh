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
	
	using Mac::FSFileRefNum;
	using Mac::FSIOPosMode;
	
	
	SInt32 FSWrite( FSFileRefNum   file,
	                FSIOPosMode    positionMode,
	                SInt32         positionOffset,
	                SInt32         requestCount,
	                const void    *buffer );
	
}

#endif

