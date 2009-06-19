/*	=====================
 *	FSMakeFSSpec_Async.hh
 *	=====================
 */

#ifndef MACIO_FSMAKEFSSPECASYNC_HH
#define MACIO_FSMAKEFSSPECASYNC_HH

// MacIO
#include "MacIO/Async.hh"
#include "MacIO/FSMakeFSSpec.hh"
#include "MacIO/Routines.hh"
#include "MacIO/ThrowOSStatus.hh"


namespace MacIO
{
	
	template < class Policy, class Callback >
	inline FSSpec FSMakeFSSpec( FSVolumeRefNum        vRefNum,
	                            FSDirID               dirID,
	                            const unsigned char*  name,
	                            Callback              callback,
	                            ::IOCompletionUPP     completion = NULL )
	{
		HParamBlockRec  pb;
		FSSpec          result;
		
		Init_PB_For_MakeFSSpec( pb, vRefNum, dirID, name, result );
		
		(void) PBAsync< MakeFSSpec_Traits, Policy >( pb,
		                                             callback,
		                                             completion );
		
		return result;
	}
	
}

#endif

