/*
	DebugStr.c
	----------
*/

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// Standard C
#include <string.h>


int main( int argc, char** argv )
{
	Str255 pstr;
	
	pstr[0] = '\0';
	
	if ( argc > 1 )
	{
		const char* str = argv[1];
		
		size_t length = strlen( str );
		
		if ( length > 255 )
		{
			return 1;
		}
		
		pstr[0] = length;
		
		memcpy( pstr + 1, str, length );
	}
	
	DebugStr( pstr );
	
	return 0;
}

