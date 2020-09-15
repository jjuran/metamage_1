/*
	StrUtils.cc
	-----------
*/

#include "StrUtils.hh"

// Mac OS
#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif

enum
{
	_PtrToHand = 0xA9E3,
};

static inline asm
StringHandle PtrToHand( const void* p : __A0, UInt32 n : __D0 )
{
	_PtrToHand
}

pascal unsigned char** NewString_patch( const unsigned char* s )
{
	return PtrToHand( s, 1 + s[ 0 ] );
}

pascal void SetString_patch( unsigned char** h, const unsigned char* s )
{
	PtrToXHand( (Ptr) s, (Handle) h, 1 + s[ 0 ] );
}
