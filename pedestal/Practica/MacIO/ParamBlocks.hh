/*	==============
 *	ParamBlocks.hh
 *	==============
 */

#ifndef MACIO_PARAMBLOCKS_HH
#define MACIO_PARAMBLOCKS_HH

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif


namespace MacIO
{
	
	struct ParamBlock_Traits
	{
		typedef ParamBlockRec PB;
		
		static ::IOCompletionUPP& IOCompletion( PB& pb )  { return pb.ioParam.ioCompletion; }
		
		static volatile ::OSErr& IOResult( PB& pb )  { return pb.ioParam.ioResult; }
	};
	
	struct HParamBlock_Traits
	{
		typedef HParamBlockRec PB;
		
		static ::IOCompletionUPP& IOCompletion( PB& pb )  { return pb.ioParam.ioCompletion; }
		
		static volatile ::OSErr& IOResult( PB& pb )  { return pb.ioParam.ioResult; }
	};
	
	struct CInfoPB_Traits
	{
		typedef CInfoPBRec PB;
		
		static ::IOCompletionUPP& IOCompletion( PB& pb )  { return pb.dirInfo.ioCompletion; }
		
		static volatile ::OSErr& IOResult( PB& pb )  { return pb.dirInfo.ioResult; }
	};
	
}

#endif

