/*
	MacFixup/MixedMode.h
	--------------------
	
	This source file contains code similar to that in Apple's MixedMode.h
	for the purpose of interoperability.  It provides essentially the same
	effect as patching the original file (and like a patch, is required to
	duplicate some of the code it's patching in order to provide context).
	
	Notwithstanding Apple's copyright on the original code, the patch itself
	was written in 2010 by Joshua Juran, who places it in the public domain.
*/

#ifndef MACFIXUP_MIXEDMODE_H
#define MACFIXUP_MIXEDMODE_H

#ifndef __MIXEDMODE__
#include <CIncludes/MixedMode.h>
#endif


#if TARGET_RT_MAC_CFM && !OPAQUE_UPP_TYPES
	
	// Nitrogen requires unique UPP types for template parameters.
	// Non-CFM UPPs are just ProcPtrs (and therefore unique).
	// Opaque UPPs are unique.
	
	#undef STACK_UPP_TYPE
	#undef REGISTER_UPP_TYPE
	
	#define    STACK_UPP_TYPE( ProcPtr )  struct Opaque ## ProcPtr ## *
	#define REGISTER_UPP_TYPE( ProcPtr )  struct Opaque ## ProcPtr ## *
	
	#undef CALL_ZERO_PARAMETER_UPP
	#undef CALL_ONE_PARAMETER_UPP
	#undef CALL_TWO_PARAMETER_UPP
	#undef CALL_THREE_PARAMETER_UPP
	#undef CALL_FOUR_PARAMETER_UPP
	#undef CALL_FIVE_PARAMETER_UPP
	#undef CALL_SIX_PARAMETER_UPP
	#undef CALL_SEVEN_PARAMETER_UPP
	#undef CALL_EIGHT_PARAMETER_UPP
	#undef CALL_NINE_PARAMETER_UPP
	#undef CALL_TEN_PARAMETER_UPP
	#undef CALL_ELEVEN_PARAMETER_UPP
	#undef CALL_TWELVE_PARAMETER_UPP
	#undef CALL_THIRTEEN_PARAMETER_UPP
	
	#define CALL_ZERO_PARAMETER_UPP(     upp, procInfo                                                         )  CallUniversalProc( (UniversalProcPtr) upp, procInfo                                                                                   )
	#define CALL_ONE_PARAMETER_UPP(      upp, procInfo, p1                                                     )  CallUniversalProc( (UniversalProcPtr) upp, procInfo, (p1)                                                                             )
	#define CALL_TWO_PARAMETER_UPP(      upp, procInfo, p1, p2                                                 )  CallUniversalProc( (UniversalProcPtr) upp, procInfo, (p1), (p2)                                                                       )
	#define CALL_THREE_PARAMETER_UPP(    upp, procInfo, p1, p2, p3                                             )  CallUniversalProc( (UniversalProcPtr) upp, procInfo, (p1), (p2), (p3)                                                                 )
	#define CALL_FOUR_PARAMETER_UPP(     upp, procInfo, p1, p2, p3, p4                                         )  CallUniversalProc( (UniversalProcPtr) upp, procInfo, (p1), (p2), (p3), (p4)                                                           )
	#define CALL_FIVE_PARAMETER_UPP(     upp, procInfo, p1, p2, p3, p4, p5                                     )  CallUniversalProc( (UniversalProcPtr) upp, procInfo, (p1), (p2), (p3), (p4), (p5)                                                     )
	#define CALL_SIX_PARAMETER_UPP(      upp, procInfo, p1, p2, p3, p4, p5, p6                                 )  CallUniversalProc( (UniversalProcPtr) upp, procInfo, (p1), (p2), (p3), (p4), (p5), (p6)                                               )
	#define CALL_SEVEN_PARAMETER_UPP(    upp, procInfo, p1, p2, p3, p4, p5, p6, p7                             )  CallUniversalProc( (UniversalProcPtr) upp, procInfo, (p1), (p2), (p3), (p4), (p5), (p6), (p7)                                         )
	#define CALL_EIGHT_PARAMETER_UPP(    upp, procInfo, p1, p2, p3, p4, p5, p6, p7, p8                         )  CallUniversalProc( (UniversalProcPtr) upp, procInfo, (p1), (p2), (p3), (p4), (p5), (p6), (p7), (p8)                                   )
	#define CALL_NINE_PARAMETER_UPP(     upp, procInfo, p1, p2, p3, p4, p5, p6, p7, p8, p9                     )  CallUniversalProc( (UniversalProcPtr) upp, procInfo, (p1), (p2), (p3), (p4), (p5), (p6), (p7), (p8), (p9)                             )
	#define CALL_TEN_PARAMETER_UPP(      upp, procInfo, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10                )  CallUniversalProc( (UniversalProcPtr) upp, procInfo, (p1), (p2), (p3), (p4), (p5), (p6), (p7), (p8), (p9), (p10)                      )
	#define CALL_ELEVEN_PARAMETER_UPP(   upp, procInfo, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11           )  CallUniversalProc( (UniversalProcPtr) upp, procInfo, (p1), (p2), (p3), (p4), (p5), (p6), (p7), (p8), (p9), (p10), (p11)               )
	#define CALL_TWELVE_PARAMETER_UPP(   upp, procInfo, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12      )  CallUniversalProc( (UniversalProcPtr) upp, procInfo, (p1), (p2), (p3), (p4), (p5), (p6), (p7), (p8), (p9), (p10), (p11), (p12)        )
	#define CALL_THIRTEEN_PARAMETER_UPP( upp, procInfo, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13 )  CallUniversalProc( (UniversalProcPtr) upp, procInfo, (p1), (p2), (p3), (p4), (p5), (p6), (p7), (p8), (p9), (p10), (p11), (p12), (p13) )
#endif  // !OPAQUE_UPP_TYPES

#endif
