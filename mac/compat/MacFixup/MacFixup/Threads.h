/*
	MacFixup/Threads.h
	------------------
	
	This source file contains code similar to that in Apple's MixedMode.h
	for the purpose of interoperability.  It provides essentially the same
	effect as patching the original file (and like a patch, is required to
	duplicate some of the code it's patching in order to provide context).
	
	Notwithstanding Apple's copyright on the original code, the patch itself
	was written in 2010 by Joshua Juran, who places it in the public domain.
*/

#ifndef MACFIXUP_THREADS_H
#define MACFIXUP_THREADS_H

// MacFixup
#include "UPP.h"


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

DEFINE_UPP_1( ThreadEntry,             voidPtr,  void*               )
DEFINE_UPP_1( ThreadScheduler,         ThreadID, SchedulerInfoRecPtr )
DEFINE_UPP_2( ThreadSwitch,            void,     ThreadID,  void*    )
DEFINE_UPP_2( ThreadTermination,       void,     ThreadID,  void*    )
DEFINE_UPP_1( DebuggerNewThread,       void,     ThreadID            )
DEFINE_UPP_1( DebuggerDisposeThread,   void,     ThreadID            )
DEFINE_UPP_1( DebuggerThreadScheduler, ThreadID, SchedulerInfoRecPtr )

#endif  // TARGET_CPU_68K  &&  TARGET_RT_MAC_CFM  && !OPAQUE_UPP_TYPES

#endif
