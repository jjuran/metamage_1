/*	===============
 *	Init_IOParam.hh
 *	===============
 */

#ifndef MACIO_INITIOPARAM_HH
#define MACIO_INITIOPARAM_HH

// Nitrogen
#include "Nitrogen/Files.h"


namespace MacIO
{
	
	using Nitrogen::FSFileRefNum;
	using Nitrogen::FSIOPosMode;
	
	
	inline void Init_PB_For_ReadWrite( ParamBlockRec&  pb,
	                                   FSFileRefNum    file,
	                                   FSIOPosMode     positionMode,
	                                   SInt32          positionOffset,
	                                   SInt32          requestCount,
	                                   const void *    buffer )
	{
		IOParam& io = pb.ioParam;
		
		io.ioRefNum    = file;
		io.ioBuffer    = (char*) buffer;
		io.ioReqCount  = requestCount;
		io.ioPosMode   = positionMode;
		io.ioPosOffset = positionOffset;
	}
	
}

#endif

