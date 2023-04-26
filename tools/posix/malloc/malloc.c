/*
	malloc.c
	--------
*/

// Standard C
#include <stdio.h>
#include <stdlib.h>


int main( int argc, char** argv )
{
	if ( argc > 1 )
	{
		unsigned long size = strtoul( argv[ 1 ], NULL, 0 );
		
		void* p = malloc( size );
		
		printf( "%p\n", p );
		
		free( p );
	}
	
	return 0;
}
