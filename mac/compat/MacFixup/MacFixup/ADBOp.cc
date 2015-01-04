/*
	ADBOp.cc
	--------
	
	Adapted from Apple DTS code by Joshua Juran
	
	Technical Q&A HW30
	Making the ADBOp call from CFM-68K
	http://developer.apple.com/legacy/mac/library/qa/hw/hw30.html
*/

// Mac OS
#ifndef __DESKBUS__
#include <DeskBus.h>
#endif
#ifndef __MIXEDMODE__
#include <MixedMode.h>
#endif
#ifndef __PATCHES__
#include <Patches.h>
#endif
#ifndef __TRAPS__
#include <Traps.h>
#endif


pascal OSErr ADBOp( Ptr               refCon,
                    ADBCompletionUPP  completion,
                    Ptr               buffer,
                    short             commandNum )
{
	ADBOpBlock block = { buffer, (ADBServiceRoutineUPP) completion, refCon };
	
	UniversalProcPtr proc = (UniversalProcPtr) NGetTrapAddress( _ADBOp, kOSTrapType );
	
	ProcInfoType procInfo = kRegisterBased
	                      | RESULT_SIZE( SIZE_CODE( sizeof (OSErr) ) )
	                      | REGISTER_RESULT_LOCATION( kRegisterD0 )
	                      | REGISTER_ROUTINE_PARAMETER( 1, kRegisterA0, SIZE_CODE( sizeof (ADBOpBlock*) ) )
	                      | REGISTER_ROUTINE_PARAMETER( 2, kRegisterD0, SIZE_CODE( sizeof (short      ) ) );
	
	OSErr err = CallUniversalProc( proc, procInfo, &block, commandNum );
	
	return err;
}
