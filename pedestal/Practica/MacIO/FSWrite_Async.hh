/*	================
 *	FSWrite_Async.hh
 *	================
 */

#ifndef MACIO_FSWRITEASYNC_HH
#define MACIO_FSWRITEASYNC_HH

// Nitrogen
#ifndef MAC_FILES_TYPES_FSFILEREFNUM_HH
#include "Mac/Files/Types/FSFileRefNum.hh"
#endif
#ifndef MAC_FILES_TYPES_FSIOPOSMODE_HH
#include "Mac/Files/Types/FSIOPosMode.hh"
#endif

// MacIO
#include "MacIO/Async.hh"
#include "MacIO/Init_IOParam.hh"
#include "MacIO/Routines.hh"
#include "MacIO/ThrowOSStatus.hh"


namespace MacIO
{
	
	template < class Callback >
	inline SInt32 FSWrite( Mac::FSFileRefNum  file,
	                       Mac::FSIOPosMode   positionMode,
	                       SInt32             positionOffset,
	                       SInt32             requestCount,
	                       const void *       buffer,
	                       Callback           callback,
	                       ::IOCompletionUPP  completion = NULL )
	{
		ParamBlockRec pb;
		
		Init_PB_For_ReadWrite( pb,
		                       file,
		                       positionMode,
		                       positionOffset,
		                       requestCount,
		                       buffer );
		
		PBAsync< Write_Traits, Throw_All >( pb,
		                                    callback,
		                                    completion );
		
		return pb.ioParam.ioActCount;
	}
	
	// Default position mode
	template < class Callback >
	inline SInt32 FSWrite( Mac::FSFileRefNum  file,
	                       SInt32             requestCount,
	                       const void *       buffer,
	                       Callback           callback,
	                       ::IOCompletionUPP  completion = NULL )
	{
		return FSWrite( file,
		                fsAtMark,
		                0,
		                requestCount,
		                buffer,
		                callback,
		                completion );
	}
	
}

#endif

