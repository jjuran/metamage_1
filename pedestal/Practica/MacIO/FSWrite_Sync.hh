/*	===============
 *	FSWrite_Sync.hh
 *	===============
 */

#ifndef MACIO_FSWRITESYNC_HH
#define MACIO_FSWRITESYNC_HH

// Nitrogen
#include "Nitrogen/Files.h"


namespace MacIO
{
	
	using Nitrogen::FSFileRefNum;
	using Nitrogen::FSIOPosMode;
	
	
	SInt32 FSWrite( FSFileRefNum   file,
	                FSIOPosMode    positionMode,
	                SInt32         positionOffset,
	                SInt32         requestCount,
	                const void    *buffer );
	
}

#endif

