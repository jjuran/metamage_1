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
#include "callouts.hh"


pascal unsigned char** NewString_patch( const unsigned char* s )
{
	const Size size = 1 + s[ 0 ];
	
	Handle h = NewHandle( size );
	
	if ( h != NULL )
	{
		fast_memcpy( *h, s, size );
	}
	
	return (StringHandle) h;
}

pascal void SetString_patch( unsigned char** h, const unsigned char* s )
{
	const Size size = 1 + s[ 0 ];
	
	if ( h != NULL )
	{
		SetHandleSize( (Handle) h, size );
		
		fast_memcpy( *h, s, size );
	}
}
