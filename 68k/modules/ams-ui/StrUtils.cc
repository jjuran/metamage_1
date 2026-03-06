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


pascal StringHandle NewString_patch( const Byte* s )
{
	return PtrToHand_StringHandle( s, 1 + s[ 0 ] );
}

pascal void SetString_patch( StringHandle h, const Byte* s )
{
	PtrToXHand( (Ptr) s, (Handle) h, 1 + s[ 0 ] );
}
