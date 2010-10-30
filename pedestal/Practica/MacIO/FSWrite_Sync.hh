/*	===============
 *	FSWrite_Sync.hh
 *	===============
 */

#ifndef MACIO_FSWRITESYNC_HH
#define MACIO_FSWRITESYNC_HH

// Nitrogen
#ifndef MAC_FILES_TYPES_FSFILEREFNUM_HH
#include "Mac/Files/Types/FSFileRefNum.hh"
#endif
#ifndef MAC_FILES_TYPES_FSIOPOSMODE_HH
#include "Mac/Files/Types/FSIOPosMode.hh"
#endif


namespace MacIO
{
	
	SInt32 FSWrite( Mac::FSFileRefNum  file,
	                Mac::FSIOPosMode   positionMode,
	                SInt32             positionOffset,
	                SInt32             requestCount,
	                const void *       buffer );
	
}

#endif

