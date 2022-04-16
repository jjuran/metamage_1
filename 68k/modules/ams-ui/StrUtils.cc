/*
	StrUtils.cc
	-----------
*/

#include "StrUtils.hh"

// Mac OS
#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif

// ams-common
#include "unglue.hh"


pascal unsigned char** NewString_patch( const unsigned char* s )
{
	return PtrToHand( s, 1 + s[ 0 ] );
}

pascal void SetString_patch( unsigned char** h, const unsigned char* s )
{
	PtrToXHand( (Ptr) s, (Handle) h, 1 + s[ 0 ] );
}
