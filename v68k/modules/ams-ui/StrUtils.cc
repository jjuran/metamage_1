/*
	StrUtils.cc
	-----------
*/

#include "StrUtils.hh"

// Mac OS
#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif


pascal unsigned char** NewString_patch( const unsigned char* s )
{
	Handle h = NULL;
	
	PtrToHand( (Ptr) s, &h, 1 + s[ 0 ] );
	
	return (StringHandle) h;
}

pascal void SetString_patch( unsigned char** h, const unsigned char* s )
{
	PtrToXHand( (Ptr) s, (Handle) h, 1 + s[ 0 ] );
}
