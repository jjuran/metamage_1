/*
	StrUtils.cc
	-----------
*/

#include "StrUtils.hh"

// Mac OS
#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// Standard C
#include <string.h>


pascal unsigned char** NewString_patch( const unsigned char* s )
{
	const Size size = 1 + s[ 0 ];
	
	Handle h = NewHandle( size );
	
	if ( h != NULL )
	{
		memcpy( *h, s, size );
	}
	
	return (StringHandle) h;
}

pascal void SetString_patch( unsigned char** h, const unsigned char* s )
{
	const Size size = 1 + s[ 0 ];
	
	if ( h != NULL )
	{
		SetHandleSize( (Handle) h, size );
		
		memcpy( *h, s, size );
	}
}

pascal unsigned char** GetString_patch( short resID )
{
	return (StringHandle) GetResource( 'STR ', resID );
}
