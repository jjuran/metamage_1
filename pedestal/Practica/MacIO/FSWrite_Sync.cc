/*	===============
 *	FSWrite_Sync.cc
 *	===============
 */

#include "MacIO/FSWrite_Sync.hh"

// MacIO
#include "MacIO/Init_IOParam.hh"
#include "MacIO/Routines.hh"
#include "MacIO/Sync.hh"
#include "MacIO/ThrowOSStatus.hh"


namespace MacIO
{
	
	SInt32 FSWrite( FSFileRefNum   file,
	                FSIOPosMode    positionMode,
	                SInt32         positionOffset,
	                SInt32         requestCount,
	                const void    *buffer )
	{
		ParamBlockRec pb;
		
		Init_PB_For_ReadWrite( pb,
		                       file,
		                       positionMode,
		                       positionOffset,
		                       requestCount,
		                       buffer );
		
		PBSync< Write_Traits, Throw_All >( pb );
		
		return pb.ioParam.ioActCount;
	}
	
}

