/*
	type-macros.h
	-------------
*/

#ifndef MACUPP_TYPEMACROS_H
#define MACUPP_TYPEMACROS_H

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif


#if TARGET_RT_MAC_CFM

/*
	To use these macros, you must first include <MixedMode.h>.
*/

#define DEFINE_UPP_TYPE( Name )  typedef struct Opaque##Name##ProcPtr* Name##UPP;

#define DEFINE_UPP_CONSTRUCTOR( Name )                                    \
inline Name##UPP New##Name##UPP( Name##ProcPtr proc )                     \
{                                                                         \
	return (Name##UPP) NewRoutineDescriptor( (ProcPtr) (proc),            \
	                                         upp##Name##ProcInfo,         \
	                                         GetCurrentArchitecture() );  \
}

#define DEFINE_UPP_DESTRUCTOR( Name )                    \
inline void Dispose##Name##UPP( Name##UPP upp )          \
{                                                        \
	DisposeRoutineDescriptor( (UniversalProcPtr) upp );  \
}

#define DEFINE_UPP_INVOKER_0( Name, R )                                                 \
inline R Invoke##Name##UPP( Name##UPP upp )                                             \
{                                                                                       \
	return (R) CALL_ZERO_PARAMETER_UPP( (UniversalProcPtr) upp, upp##Name##ProcInfo );  \
}

#define DEFINE_UPP_VOID_INVOKER_0( Name )                                    \
inline void Invoke##Name##UPP( Name##UPP upp )                               \
{                                                                            \
	CALL_ZERO_PARAMETER_UPP( (UniversalProcPtr) upp, upp##Name##ProcInfo );  \
}

#define DEFINE_UPP_INVOKER_1( Name, R, P1 )                                                \
inline R Invoke##Name##UPP( P1 p1, Name##UPP upp )                                         \
{                                                                                          \
	return (R) CALL_ONE_PARAMETER_UPP( (UniversalProcPtr) upp, upp##Name##ProcInfo, p1 );  \
}

#define DEFINE_UPP_VOID_INVOKER_1( Name, P1 )                                   \
inline void Invoke##Name##UPP( P1 p1, Name##UPP upp )                           \
{                                                                               \
	CALL_ONE_PARAMETER_UPP( (UniversalProcPtr) upp, upp##Name##ProcInfo, p1 );  \
}

#define DEFINE_UPP_INVOKER_2( Name, R, P1, P2 )                                                \
inline R Invoke##Name##UPP( P1 p1, P2 p2, Name##UPP upp )                                      \
{                                                                                              \
	return (R) CALL_TWO_PARAMETER_UPP( (UniversalProcPtr) upp, upp##Name##ProcInfo, p1, p2 );  \
}

#define DEFINE_UPP_VOID_INVOKER_2( Name, P1, P2 )                                   \
inline void Invoke##Name##UPP( P1 p1, P2 p2, Name##UPP upp )                        \
{                                                                                   \
	CALL_TWO_PARAMETER_UPP( (UniversalProcPtr) upp, upp##Name##ProcInfo, p1, p2 );  \
}

#else  // #if TARGET_RT_MAC_CFM

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

#define DEFINE_UPP_INVOKER_1( Name, R, P1 )         \
inline R Invoke##Name##UPP( P1 p1, Name##UPP upp )  \
{                                                   \
	return upp( p1 );                               \
}

#define DEFINE_UPP_INVOKER_2( Name, R, P1, P2 )            \
inline R Invoke##Name##UPP( P1 p1, P2 p2, Name##UPP upp )  \
{                                                          \
	return upp( p1, p2 );                                  \
}

#endif  // #else  // #if TARGET_RT_MAC_CFM

#define DEFINE_UPP_0( Name, R )         \
        DEFINE_UPP_TYPE( Name )         \
        DEFINE_UPP_CONSTRUCTOR( Name )  \
        DEFINE_UPP_DESTRUCTOR( Name )   \
        DEFINE_UPP_INVOKER_0( Name, R )

#define DEFINE_UPP_1( Name, R, P1 )     \
        DEFINE_UPP_TYPE( Name )         \
        DEFINE_UPP_CONSTRUCTOR( Name )  \
        DEFINE_UPP_DESTRUCTOR( Name )   \
        DEFINE_UPP_INVOKER_1( Name, R, P1 )

#define DEFINE_UPP_2( Name, R, P1, P2 )  \
        DEFINE_UPP_TYPE( Name )          \
        DEFINE_UPP_CONSTRUCTOR( Name )   \
        DEFINE_UPP_DESTRUCTOR( Name )    \
        DEFINE_UPP_INVOKER_2( Name, R, P1, P2 )

#endif
