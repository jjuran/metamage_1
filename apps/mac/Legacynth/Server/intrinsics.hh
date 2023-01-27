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
asm UInt32 SWAP( UInt32 x : __D0 )
{
	SWAP     D0
}

static inline
asm
SInt32 MULS( SInt16 a : __D0, SInt16 b : __D1 )
{
	MULS.W   D1,D0
}

static inline
asm
UInt32 MULU( UInt16 a : __D0, UInt16 b : __D1 )
{
	MULU.W   D1,D0
}

#else  // #ifdef __MC68K__

static inline
UInt32 SWAP( UInt32 x )
{
	return x << 16 | x >> 16;
}

static inline
SInt32 MULS( SInt16 a, SInt16 b )
{
	return a * b;
}

static inline
UInt32 MULU( UInt16 a, UInt16 b )
{
	return a * b;
}

#endif  // #else  // #ifdef __MC68K__

#endif
