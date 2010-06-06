/*	==============
 *	FSRead_Sync.hh
 *	==============
 */

#ifndef MACIO_FSREADSYNC_HH
#define MACIO_FSREADSYNC_HH

// Nitrogen
#include "Mac/Files/Types/FSFileRefNum.hh"
#include "Mac/Files/Types/FSIOPosMode.hh"

// MacIO
#include "MacIO/EOF_Policy.hh"


namespace MacIO
{
	
	using Mac::FSFileRefNum;
	using Mac::FSIOPosMode;
	
	
	SInt32 FSRead( EOF_Policy    policy,
	               FSFileRefNum  file,
	               FSIOPosMode   positionMode,
	               SInt32        positionOffset,
	               SInt32        requestCount,
	               void *        buffer );
	
	template < EOF_Policy policy >
	inline SInt32 FSRead( FSFileRefNum  file,
	                      FSIOPosMode   positionMode,
	                      SInt32        positionOffset,
	                      SInt32        requestCount,
	                      void *        buffer )
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

