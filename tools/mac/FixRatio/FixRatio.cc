/*
	FixRatio.cc
	-----------
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


using quickdraw::fix_ratio;


static
void report( short a, short b )
{
	printf( "input: $%.8x / $%.8x | %d / %d\n", a, b, a, b );
	
	const int original = FixRatio ( a, b );
	const int copyleft = fix_ratio( a, b );
	
	printf( "original QD: $%.8x | %d\n", original, original );
	printf( "copyleft qd: $%.8x | %d\n", copyleft, copyleft );
}

int main( int argc, char** argv )
{
	if ( argc > 3  ||  argc == 2 )
	{
		return 2;
	}
	
	if ( argc == 3 )
	{
		const int a = atoi( argv[ 1 ] );
		const int b = atoi( argv[ 2 ] );
		
		report( a, b );
		
		return 0;
	}
	
	unsigned long n_failures = 0;
	
	for ( unsigned x = 1;  x != 0;  ++x )
	{
		if ( (x & 0x00FFFFFF) == 0 )
		{
			write( 1, ".", 1 );
		}
		
		const short a = x >> 16;
		const short b = x;
		
		if ( fix_ratio( a, b ) != FixRatio( a, b ) )
		{
			++n_failures;
			
			//report( a, b );
			//exit( 1 );
		}
	}
	
	if ( n_failures )
	{
		printf( "%lu failures\n", n_failures );
	}
	
	write( 1, "\n", 1 );
	
	return 0;
}
