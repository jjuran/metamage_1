/*	==============
 *	FSRead_Sync.hh
 *	==============
 */

#ifndef MACIO_FSREADSYNC_HH
#define MACIO_FSREADSYNC_HH

// Nitrogen
#ifndef MAC_FILES_TYPES_FSFILEREFNUM_HH
#include "Mac/Files/Types/FSFileRefNum.hh"
#endif
#ifndef MAC_FILES_TYPES_FSIOPOSMODE_HH
#include "Mac/Files/Types/FSIOPosMode.hh"
#endif

// MacIO
#include "MacIO/EOF_Policy.hh"


namespace MacIO
{
	
	SInt32 FSRead( EOF_Policy         policy,
	               Mac::FSFileRefNum  file,
	               Mac::FSIOPosMode   positionMode,
	               SInt32             positionOffset,
	               SInt32             requestCount,
	               void *             buffer );
	
	template < EOF_Policy policy >
	inline SInt32 FSRead( Mac::FSFileRefNum  file,
	                      Mac::FSIOPosMode   positionMode,
	                      SInt32             positionOffset,
	                      SInt32             requestCount,
	                      void *             buffer )
	{
		return FSRead( policy,
		               file,
		               positionMode,
		               positionOffset,
		               requestCount,
		               buffer );
	}
}

#endif

