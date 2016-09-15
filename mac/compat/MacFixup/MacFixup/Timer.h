/*
	MacFixup/Timer.h
	----------------
	
	This source file contains code similar to that in Apple's MixedMode.h
	for the purpose of interoperability.  It provides essentially the same
	effect as patching the original file (and like a patch, is required to
	duplicate some of the code it's patching in order to provide context).
	
	Notwithstanding Apple's copyright on the original code, the patch itself
	was written in 2010 by Joshua Juran, who places it in the public domain.
*/

#ifndef MACFIXUP_TIMER_H
#define MACFIXUP_TIMER_H

// Mac OS
#ifndef __MIXEDMODE__
#include <MixedMode.h>
#endif

// mac-upp-utils
#include "mac_upp/type-macros.h"


#if TARGET_RT_MAC_CFM  &&  !OPAQUE_UPP_TYPES

#ifdef __TIMER__
#error Timer.h included too early to override broken definitions
#endif

// Temporarily turn on OPAQUE_UPP_TYPES so we don't get the bad definitions
#undef OPAQUE_UPP_TYPES
#define OPAQUE_UPP_TYPES 2

// Temporarily redefine TVECTOR_UPP_TYPE so we get distinct UPP types
#undef TVECTOR_UPP_TYPE
#define TVECTOR_UPP_TYPE( ProcPtr )  STACK_UPP_TYPE( ProcPtr )

#endif  // TARGET_RT_MAC_CFM  &&  !OPAQUE_UPP_TYPES


#ifndef __TIMER__
#include <CIncludes/Timer.h>
#endif


#if TARGET_RT_MAC_CFM  && OPAQUE_UPP_TYPES == 2

// Restore OPAQUE_UPP_TYPES
#undef OPAQUE_UPP_TYPES
#define OPAQUE_UPP_TYPES 0

// Restore TVECTOR_UPP_TYPE
#undef TVECTOR_UPP_TYPE
#define TVECTOR_UPP_TYPE( ProcPtr )  ProcPtr

// Define the procInfo constants that we missed with OPAQUE_UPP_TYPES on
enum
{
	uppTimerProcInfo = 0x0000B802
};

DEFINE_UPP_CONSTRUCTOR( Timer )
DEFINE_UPP_DESTRUCTOR( Timer )
DEFINE_UPP_VOID_INVOKER_1( Timer, TMTaskPtr )

#endif  // TARGET_RT_MAC_CFM  && !OPAQUE_UPP_TYPES

#endif
