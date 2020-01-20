/*
	AngleFromSlope.cc
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
#include <signal.h>
#include <stdio.h>

// gear
#include "gear/hexadecimal.hh"


static
void print_slopes()
{
	short last_angle = 0;
	
	UInt32 slope = 0;
	
	do
	{
		short angle = AngleFromSlope( slope );
		
		if ( angle != last_angle )
		{
			last_angle = angle;
			
			printf( "%.8x: %d\n", slope, angle );
			fflush( stdout );
		}
		
		++slope;
		
		if ( (short) slope == 0 )
		{
			kill( 1, 0 );  // allow interruption
		}
	}
	while ( last_angle != 90 );
}

int main( int argc, char** argv )
{
	if ( argc >= 1 + 1 )
	{
		const char* s = argv[ 1 ];
		
		const Fixed slope = gear::decode_32_bit_hex( s );
		
		const short angle = AngleFromSlope( slope );
		
		printf( "%d\n", angle );
	}
	else
	{
		print_slopes();
	}
	
	return 0;
}
