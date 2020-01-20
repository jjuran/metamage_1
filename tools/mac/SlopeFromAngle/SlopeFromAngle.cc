/*
	SlopeFromAngle.cc
	-----------------
*/

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// Standard C
#include <stdio.h>

// gear
#include "gear/parse_decimal.hh"


static
void print_slopes()
{
	for ( int i = -720;  i <= 720;  ++i )
	{
		printf( "%d: %.8x\n", i, SlopeFromAngle( i ) );
	}
}

int main( int argc, char** argv )
{
	if ( argc >= 1 + 1 )
	{
		const char* s = argv[ 1 ];
		
		const int angle = gear::parse_decimal( s );
		
		const Fixed slope = SlopeFromAngle( angle );
		
		printf( "%.8x\n", slope );
	}
	else
	{
		print_slopes();
	}
	
	return 0;
}
