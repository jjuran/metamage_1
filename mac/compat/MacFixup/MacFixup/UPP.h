/*
	MacFixup/UPP.h
	--------------
*/

#ifndef MACFIXUP_UPP_H
#define MACFIXUP_UPP_H

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __MIXEDMODE__
#include <MixedMode.h>
#endif


#if TARGET_RT_MAC_CFM

#define DEFINE_UPP_TYPE( Name )  typedef struct Opaque##Name##ProcPtr* Name##UPP;

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

#define DEFINE_UPP_INVOKER_0( Name, R )                                                 \
inline R Invoke##Name##UPP( Name##UPP upp )                                             \
{                                                                                       \
	return (R) CALL_ZERO_PARAMETER_UPP( (UniversalProcPtr) upp, upp##Name##ProcInfo );  \
}

#else

#define DEFINE_UPP_TYPE( Name )  typedef Name##ProcPtr Name##UPP;

#define DEFINE_UPP_CONSTRUCTOR( Name )                 \
inline Name##UPP New##Name##UPP( Name##ProcPtr proc )  \
{                                                      \
	return proc;                                       \
}

#define DEFINE_UPP_DESTRUCTOR( Name )           \
inline void Dispose##Name##UPP( Name##UPP upp)  \
{                                               \
}

#define DEFINE_UPP_INVOKER_0( Name, R )      \
inline R Invoke##Name##UPP( Name##UPP upp )  \
{                                            \
	return upp();                            \
}

#endif  // #if TARGET_RT_MAC_CFM

#define DEFINE_UPP_0( Name, R )        \
        DEFINE_UPP_TYPE( Name )        \
        DEFINE_UPP_CONSTRUCTOR( Name ) \
        DEFINE_UPP_DESTRUCTOR( Name )  \
        DEFINE_UPP_INVOKER_0( Name, R )

#endif
