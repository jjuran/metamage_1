/*
	MacFixup/Threads.h
	------------------
	
	This source file contains code similar to that from Apple's MixedMode.h for
	the purpose of interoperability.  It provides essentially the same effect as
	patching the original file (and like a patch, is required to duplicate some
	of the code it's patching in order to provide context).
	
	Notwithstanding Apple's copyright on the original code, the patch itself was
	written in 2010 by Joshua Juran, who places it in the public domain.
*/

#ifndef MACFIXUP_THREADS_H
#define MACFIXUP_THREADS_H

#ifndef __MIXEDMODE__
#include <MixedMode.h>
#endif


#if TARGET_CPU_68K  &&  TARGET_RT_MAC_CFM  &&  !OPAQUE_UPP_TYPES

#ifdef __THREADS__
#error Threads.h included too early to override broken definitions
#endif

// Temporarily turn on OPAQUE_UPP_TYPES so we don't get the bad definitions
#undef OPAQUE_UPP_TYPES
#define OPAQUE_UPP_TYPES 2

// Temporarily redefine TVECTOR_UPP_TYPE so we get distinct UPP types
#undef TVECTOR_UPP_TYPE
#define TVECTOR_UPP_TYPE( ProcPtr )  STACK_UPP_TYPE( ProcPtr )

#endif  // TARGET_CPU_68K  &&  TARGET_RT_MAC_CFM  &&  !OPAQUE_UPP_TYPES


#ifndef __THREADS__
#include <CIncludes/Threads.h>
#endif


#if TARGET_CPU_68K  &&  TARGET_RT_MAC_CFM  && OPAQUE_UPP_TYPES == 2

// Restore OPAQUE_UPP_TYPES
#undef OPAQUE_UPP_TYPES
#define OPAQUE_UPP_TYPES 0

// Restore TVECTOR_UPP_TYPE
#undef TVECTOR_UPP_TYPE
#define TVECTOR_UPP_TYPE( ProcPtr )  ProcPtr

// Define the procInfo constants that we missed with OPAQUE_UPP_TYPES on
enum
{
	uppThreadEntryProcInfo             = 0x000000F0,
	uppThreadSchedulerProcInfo         = 0x000000F0,
	uppThreadSwitchProcInfo            = 0x000003C0,
	uppThreadTerminationProcInfo       = 0x000003C0,
	uppDebuggerNewThreadProcInfo       = 0x000000C0,
	uppDebuggerDisposeThreadProcInfo   = 0x000000C0,
	uppDebuggerThreadSchedulerProcInfo = 0x000000F0
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

#define DEFINE_UPP_INVOKER_2( Name, R, P1, P2 )                                                \
inline R Invoke##Name##UPP( P1 p1, P2 p2, Name##UPP upp )                                     \
{                                                                                              \
	return (R) CALL_TWO_PARAMETER_UPP( (UniversalProcPtr) upp, upp##Name##ProcInfo, p1, p1 );  \
}

DEFINE_UPP_CONSTRUCTOR( ThreadEntry             )
DEFINE_UPP_CONSTRUCTOR( ThreadScheduler         )
DEFINE_UPP_CONSTRUCTOR( ThreadSwitch            )
DEFINE_UPP_CONSTRUCTOR( ThreadTermination       )
DEFINE_UPP_CONSTRUCTOR( DebuggerNewThread       )
DEFINE_UPP_CONSTRUCTOR( DebuggerDisposeThread   )
DEFINE_UPP_CONSTRUCTOR( DebuggerThreadScheduler )

DEFINE_UPP_DESTRUCTOR( ThreadEntry             )
DEFINE_UPP_DESTRUCTOR( ThreadScheduler         )
DEFINE_UPP_DESTRUCTOR( ThreadSwitch            )
DEFINE_UPP_DESTRUCTOR( ThreadTermination       )
DEFINE_UPP_DESTRUCTOR( DebuggerNewThread       )
DEFINE_UPP_DESTRUCTOR( DebuggerDisposeThread   )
DEFINE_UPP_DESTRUCTOR( DebuggerThreadScheduler )

DEFINE_UPP_INVOKER_1( ThreadEntry,             voidPtr,  void*               )
DEFINE_UPP_INVOKER_1( ThreadScheduler,         ThreadID, SchedulerInfoRecPtr )
DEFINE_UPP_INVOKER_2( ThreadSwitch,            void,     ThreadID,  void*    )
DEFINE_UPP_INVOKER_2( ThreadTermination,       void,     ThreadID,  void*    )
DEFINE_UPP_INVOKER_1( DebuggerNewThread,       void,     ThreadID            )
DEFINE_UPP_INVOKER_1( DebuggerDisposeThread,   void,     ThreadID            )
DEFINE_UPP_INVOKER_1( DebuggerThreadScheduler, ThreadID, SchedulerInfoRecPtr )

#undef DEFINE_UPP_CONSTRUCTOR
#undef DEFINE_UPP_DESTRUCTOR

#undef DEFINE_UPP_INVOKER_1
#undef DEFINE_UPP_INVOKER_2

#else  // defined(__cplusplus)

#define NewThreadEntryUPP(             proc ) (ThreadEntryUPP)             NewRoutineDescriptor( (ProcPtr) (proc), uppThreadEntryProcInfo,             GetCurrentArchitecture() )
#define NewThreadSchedulerUPP(         proc ) (ThreadSchedulerUPP)         NewRoutineDescriptor( (ProcPtr) (proc), uppThreadSchedulerProcInfo,         GetCurrentArchitecture() )
#define NewThreadSwitchUPP(            proc ) (ThreadSwitchUPP)            NewRoutineDescriptor( (ProcPtr) (proc), uppThreadSwitchProcInfo,            GetCurrentArchitecture() )
#define NewThreadTerminationUPP(       proc ) (ThreadTerminationUPP)       NewRoutineDescriptor( (ProcPtr) (proc), uppThreadTerminationProcInfo,       GetCurrentArchitecture() )
#define NewDebuggerNewThreadUPP(       proc ) (DebuggerNewThreadUPP)       NewRoutineDescriptor( (ProcPtr) (proc), uppDebuggerNewThreadProcInfo,       GetCurrentArchitecture() )
#define NewDebuggerDisposeThreadUPP(   proc ) (DebuggerDisposeThreadUPP)   NewRoutineDescriptor( (ProcPtr) (proc), uppDebuggerDisposeThreadProcInfo,   GetCurrentArchitecture() )
#define NewDebuggerThreadSchedulerUPP( proc ) (DebuggerThreadSchedulerUPP) NewRoutineDescriptor( (ProcPtr) (proc), uppDebuggerThreadSchedulerProcInfo, GetCurrentArchitecture() )

#define DisposeThreadEntryUPP(             upp )  DisposeRoutineDescriptor( (UniversalProcPtr) upp )
#define DisposeThreadSchedulerUPP(         upp )  DisposeRoutineDescriptor( (UniversalProcPtr) upp )
#define DisposeThreadSwitchUPP(            upp )  DisposeRoutineDescriptor( (UniversalProcPtr) upp )
#define DisposeThreadTerminationUPP(       upp )  DisposeRoutineDescriptor( (UniversalProcPtr) upp )
#define DisposeDebuggerNewThreadUPP(       upp )  DisposeRoutineDescriptor( (UniversalProcPtr) upp )
#define DisposeDebuggerDisposeThreadUPP(   upp )  DisposeRoutineDescriptor( (UniversalProcPtr) upp )
#define DisposeDebuggerThreadSchedulerUPP( upp )  DisposeRoutineDescriptor( (UniversalProcPtr) upp )

#define InvokeThreadEntryUPP(             param,         upp )  (voidPtr)  CALL_ONE_PARAMETER_UPP( (UniversalProcPtr) (upp), uppThreadEntryProcInfo,             (param)         )
#define InvokeThreadSchedulerUPP(         schedulerInfo, upp )  (ThreadID) CALL_ONE_PARAMETER_UPP( (UniversalProcPtr) (upp), uppThreadSchedulerProcInfo,         (schedulerInfo) )
#define InvokeThreadSwitchUPP(            thread, param, upp )  (void)     CALL_TWO_PARAMETER_UPP( (UniversalProcPtr) (upp), uppThreadSwitchProcInfo,            (thread, param) )
#define InvokeThreadTerminationUPP(       thread, param, upp )  (void)     CALL_TWO_PARAMETER_UPP( (UniversalProcPtr) (upp), uppThreadTerminationProcInfo,       (thread, param) )
#define InvokeDebuggerNewThreadUPP(       thread,        upp )  (void)     CALL_ONE_PARAMETER_UPP( (UniversalProcPtr) (upp), uppDebuggerNewThreadProcInfo,       (thread)        )
#define InvokeDebuggerDisposeThreadUPP(   thread,        upp )  (void)     CALL_ONE_PARAMETER_UPP( (UniversalProcPtr) (upp), uppDebuggerNewThreadProcInfo,       (thread)        )
#define InvokeDebuggerThreadSchedulerUPP( schedulerInfo, upp )  (ThreadID) CALL_ONE_PARAMETER_UPP( (UniversalProcPtr) (upp), uppDebuggerThreadSchedulerProcInfo, (schedulerInfo) )


#endif  // defined(__cplusplus)

#endif  // TARGET_CPU_68K  &&  TARGET_RT_MAC_CFM  && !OPAQUE_UPP_TYPES

#endif

