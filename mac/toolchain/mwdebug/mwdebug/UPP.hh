/*
	UPP.hh
	------
*/

#ifndef MWDEBUG_UPP_HH
#define MWDEBUG_UPP_HH

// Mac OS
#ifndef __CODEFRAGMENTS__
#include <CodeFragments.h>
#endif
#ifndef __MIXEDMODE__
#include <MixedMode.h>
#endif

// mac-upp-utils
#if ! TARGET_API_MAC_CARBON
#include "mac_upp/type-macros.h"
#endif


#if TARGET_API_MAC_CARBON

typedef long (*CallUniversalProc_f)( UniversalProcPtr p, ProcInfoType i, ... );

CallUniversalProc_f Get_CallUniversalProc();

#define CallUniversalProc (*Get_CallUniversalProc())

#define DEFINE_UPP_TYPE( Name )  typedef struct Opaque##Name##ProcPtr* Name##UPP;

#define DEFINE_UPP_INVOKER_0( Name, R )                                                 \
inline R Invoke##Name##UPP( Name##UPP upp )                                             \
{                                                                                       \
	return (R) CALL_ZERO_PARAMETER_UPP( (UniversalProcPtr) upp, upp##Name##ProcInfo );  \
}

#define DEFINE_UPP_0( Name, R )        \
        DEFINE_UPP_TYPE( Name )        \
        DEFINE_UPP_INVOKER_0( Name, R )

#endif

#endif
