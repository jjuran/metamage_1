/*	================
 *	FSWrite_Async.hh
 *	================
 */

#ifndef MACIO_FSWRITEASYNC_HH
#define MACIO_FSWRITEASYNC_HH

// Nitrogen
#include "Mac/Files/Types/FSFileRefNum.hh"
#include "Mac/Files/Types/FSIOPosMode.hh"

// MacIO
#include "MacIO/Async.hh"
#include "MacIO/Init_IOParam.hh"
#include "MacIO/Routines.hh"
#include "MacIO/ThrowOSStatus.hh"


namespace MacIO
{
	
	using Mac::FSFileRefNum;
	using Mac::FSIOPosMode;
	
	
	template < class Callback >
	inline SInt32 FSWrite( FSFileRefNum       file,
	                       FSIOPosMode        positionMode,
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
	inline SInt32 FSWrite( FSFileRefNum       file,
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

