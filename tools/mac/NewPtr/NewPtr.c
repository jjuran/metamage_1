/*
	NewPtr.c
	--------
*/

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif

// Standard C
#include <stdio.h>
#include <stdlib.h>


int main( int argc, char** argv )
{
	if ( argc > 1 )
	{
		unsigned long size = strtoul( argv[ 1 ], NULL, 0 );
		
		Ptr p = NewPtr( size );
		
		printf( "%p\n", p );
		
		if ( p )
		{
			DisposePtr( p );
		}
	}
	
	return 0;
}
