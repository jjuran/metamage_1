/*
	FixRound.cc
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


using quickdraw::fix_round;


static
void report( int x )
{
	printf( "input: %d\n", x );
	
	const int original = FixRound ( x );
	const int copyleft = fix_round( x );
	
	printf( "original QD: %d\n", original );
	printf( "copyleft qd: %d\n", copyleft );
}

int main( int argc, char** argv )
{
	if ( argc > 2 )
	{
		return 2;
	}
	
	if ( argc == 2 )
	{
		const int x = atoi( argv[ 1 ] );
		
		report( x );
		
		return 0;
	}
	
	unsigned long n_failures = 0;
	
	if ( fix_round( 0 ) != FixRound( 0 ) )
	{
		++n_failures;
	}
	
	for ( unsigned x = 1;  x != 0;  ++x )
	{
		if ( (x & 0x00FFFFFF) == 0 )
		{
			write( 1, ".", 1 );
		}
		
		if ( fix_round( x ) != FixRound( x ) )
		{
			++n_failures;
			
			//report( x );
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
