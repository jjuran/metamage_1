/*
	FixDiv.cc
	---------
*/

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FIXMATH__
#include <FixMath.h>
#endif

// Standard C
#include <stdio.h>
#include <stdlib.h>

// quickdraw
#include "qd/fixed.hh"


using quickdraw::fix_div;


static
void report( int a, int b )
{
	printf( "input: $%.8x / $%.8x | %d / %d\n", a, b, a, b );
	
	const int original = FixDiv ( a, b );
	const int copyleft = fix_div( a, b );
	
	printf( "original QD: $%.8x | %d\n", original, original );
	printf( "copyleft qd: $%.8x | %d\n", copyleft, copyleft );
}

int main( int argc, char** argv )
{
	if ( argc == 3 )
	{
		const int a = atoi( argv[ 1 ] );
		const int b = atoi( argv[ 2 ] );
		
		report( a, b );
		
		return 0;
	}
	
	return 2;
}
