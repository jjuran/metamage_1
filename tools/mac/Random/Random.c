/*
	Random.c
	--------
*/

// Mac OS
#include <Quickdraw.h>

// Standard C
#include <stdio.h>
#include <stdlib.h>


QDGlobals qd;


int main( int argc, char** argv )
{
	InitGraf( &qd.thePort );
	
	if ( argc > 1 )
	{
		qd.randSeed = atoi( argv[ 1 ] );
	}
	
	printf( "%d\n", Random() );
	
	return 0;
}
