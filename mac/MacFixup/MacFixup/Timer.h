/*
	MacFixup/Timer.h
	----------------
	
	This source file contains code similar to that from Apple's MixedMode.h for
	the purpose of interoperability.  It provides essentially the same effect as
	patching the original file (and like a patch, is required to duplicate some
	of the code it's patching in order to provide context).
	
	Notwithstanding Apple's copyright on the original code, the patch itself was
	written in 2010 by Joshua Juran, who places it in the public domain.
*/

#ifndef MACFIXUP_TIMER_H
#define MACFIXUP_TIMER_H

#ifndef __MIXEDMODE__
#include <MixedMode.h>
#endif


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

#ifdef __cplusplus

#define DEFINE_UPP_CONSTRUCTOR( Name )                                    \
inline Name##UPP New##Name##UPP( Name##ProcPtr proc )                     \
{                                                                         \
	return (Name##UPP) NewRoutineDescriptor( (ProcPtr) (proc),            \
	                                         upp##Name##ProcInfo,         \
	                                         GetCurrentArchitecture() );  \
}

#define DEFINE_UPP_DESTRUCTOR( Name )                    \
inline void Dispose##Name##UPP( Name##UPP upp)           \
{                                                        \
	DisposeRoutineDescriptor( (UniversalProcPtr) upp );  \
}

#define DEFINE_UPP_INVOKER_1( Name, R, P1 )                                                \
inline R Invoke##Name##UPP( P1 p1, Name##UPP upp )                                         \
{                                                                                          \
	return (R) CALL_ONE_PARAMETER_UPP( (UniversalProcPtr) upp, upp##Name##ProcInfo, p1 );  \
}

DEFINE_UPP_CONSTRUCTOR( Timer )

DEFINE_UPP_DESTRUCTOR( Timer )

DEFINE_UPP_INVOKER_1( Timer, void, TMTaskPtr )

#undef DEFINE_UPP_CONSTRUCTOR
#undef DEFINE_UPP_DESTRUCTOR

#undef DEFINE_UPP_INVOKER_1

#else  // defined(__cplusplus)

#define NewTimerUPP( proc ) (TimerUPP) NewRoutineDescriptor( (ProcPtr) (proc), uppTimerProcInfo, GetCurrentArchitecture() )

#define DisposeTimerUPP( upp )  DisposeRoutineDescriptor( (UniversalProcPtr) upp )

#define InvokeTimerUPP( tmTaskPtr, upp )  (void) CALL_ONE_PARAMETER_UPP( (UniversalProcPtr) (upp), uppTimerProcInfo, (tmTaskPtr) )


#endif  // defined(__cplusplus)

#endif  // TARGET_RT_MAC_CFM  && !OPAQUE_UPP_TYPES

#endif

