/*	=====
 *	qd.cc
 *	=====
 */

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// Standard C/C++
#include <cstddef>
#include <cstdlib>
#include <cstdio>


static int Usage()
{
	std::printf( "%s\n", "qd: command params" );
	
	return 1;
}

static short ConvertColor( const char* str )
{
	double fraction = std::atof( str );
	
	UInt16 level = fraction * 65535;
	
	return level;
}

int main( int argc, char const* const argv[] )
{
	if ( argc < 2 )
	{
		return Usage();
	}
	
	// check command
	
	if ( argc < 4 )
	{
		std::printf( "%s\n", "pt requires x, y args" );
		
		return 1;
	}
	
	short x = std::atoi( argv[ 2 ] );
	short y = std::atoi( argv[ 3 ] );
	
	RGBColor color = { 0, 0, 0 };  // Black
	
	if ( argc >= 7 )
	{
		color.red   = ConvertColor( argv[ 4 ] );
		color.green = ConvertColor( argv[ 5 ] );
		color.blue  = ConvertColor( argv[ 6 ] );
	}
	
	if ( argc > 7 )
	{
		std::fprintf( stderr, "Warning: %d unused args\n", argv - 7 );
	}
	
	::SetCPixel( x, y, &color );
	
	return 0;
}

