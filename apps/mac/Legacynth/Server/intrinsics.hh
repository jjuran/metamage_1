/*
	intrinsics.hh
	-------------
*/

#ifndef INTRINSICS_HH
#define INTRINSICS_HH

// Mac OS
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif


#ifdef __MC68K__

static inline
asm
UInt32 MULU( UInt16 a : __D0, UInt16 b : __D1 )
{
	MULU.W   D1,D0
}

#else  // #ifdef __MC68K__

static inline
UInt32 MULU( UInt16 a, UInt16 b )
{
	return a * b;
}

#endif  // #else  // #ifdef __MC68K__

#endif
