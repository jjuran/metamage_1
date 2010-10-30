/*	===============
 *	Init_IOParam.hh
 *	===============
 */

#ifndef MACIO_INITIOPARAM_HH
#define MACIO_INITIOPARAM_HH

// Nitrogen
#ifndef MAC_FILES_TYPES_FSFILEREFNUM_HH
#include "Mac/Files/Types/FSFileRefNum.hh"
#endif
#ifndef MAC_FILES_TYPES_FSIOPOSMODE_HH
#include "Mac/Files/Types/FSIOPosMode.hh"
#endif


namespace MacIO
{
	
	inline void Init_PB_For_ReadWrite( ParamBlockRec&     pb,
	                                   Mac::FSFileRefNum  file,
	                                   Mac::FSIOPosMode   positionMode,
	                                   SInt32             positionOffset,
	                                   SInt32             requestCount,
	                                   const void *       buffer )
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

