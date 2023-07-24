/*
	PatchUPPs.hh
	------------
*/

#ifndef PATCHUPPS_HH
#define PATCHUPPS_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __MIXEDMODE__
#include <MixedMode.h>
#endif

// mac-upp-utils
#include "mac_upp/type-macros.h"


enum
{
	uppStillDownPatchProcInfo = kPascalStackBased
	                          | RESULT_SIZE( SIZE_CODE( sizeof (Boolean) ) ),
};

typedef pascal Boolean (*StillDownPatchProcPtr)();

typedef STACK_UPP_TYPE( StillDownPatchProcPtr )  StillDownPatchUPP;

#if CALL_NOT_IN_CARBON

DEFINE_UPP_CONSTRUCTOR( StillDownPatch )
DEFINE_UPP_DESTRUCTOR( StillDownPatch )
DEFINE_UPP_INVOKER_0( StillDownPatch, Boolean )

#endif  // #if CALL_NOT_IN_CARBON

#endif
