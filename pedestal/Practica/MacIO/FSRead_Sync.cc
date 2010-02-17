/*	==============
 *	FSRead_Sync.cc
 *	==============
 */

#include "MacIO/FSRead_Sync.hh"

// MacIO
#include "MacIO/Init_IOParam.hh"
#include "MacIO/Routines.hh"
#include "MacIO/Sync.hh"
#include "MacIO/ThrowOSStatus.hh"


namespace MacIO
{
	
	namespace N = Nitrogen;
	
	
	SInt32 FSRead( EOF_Policy    policy,
	               FSFileRefNum  file,
	               FSIOPosMode   positionMode,
	               SInt32        positionOffset,
	               SInt32        requestCount,
	               void *        buffer )
	{
		ParamBlockRec pb;
		
		Init_PB_For_ReadWrite( pb,
		                       file,
		                       positionMode,
		                       positionOffset,
		                       requestCount,
		                       buffer );
		
		const bool ok = PBSync< Read_Traits, Return_EOF >( pb );
		
		if ( ok + !!pb.ioParam.ioActCount < policy )
		{
			N::ThrowOSStatus( eofErr );
		}
		
		return pb.ioParam.ioActCount;
	}
	
}

