/*
	rgb-color-bars.cc
	-----------------
*/

// POSIX
#include <unistd.h>

// Standard C
#include <stdint.h>
#include <string.h>

// rgb-color-bars
#include "fb.hh"


#define PROGRAM  "rgb-color-bars"

#define STR_LEN( s )  "" s, (sizeof s - 1)


static
uint32_t* fill( uint32_t* dest, size_t n, uint32_t value )
{
	while ( n-- > 0 )
	{
		*dest++ = value;
	}
	
	return dest;
}

int main( int argc, char** argv )
{
	fb::handle fbh;
	
	fb_var_screeninfo var_info = get_var_screeninfo( fbh );
	
	if ( var_info.bits_per_pixel != 32 )
	{
		write( STDERR_FILENO, STR_LEN( PROGRAM ": 32-bit depth required\n" ) );
		return 1;
	}
	
	const int w = var_info.xres;
	const int h = var_info.yres;
	
	fb::map map( fbh );
	
	char* base = (char*) map.get_base();
	
	const uint32_t colors[] =
	{
		0xFFFFFFFF,  // white
		0xFFFFFF00,  // yellow
		0xFF00FFFF,  // cyan
		0xFF00FF00,  // green
		0xFFFF00FF,  // magenta
		0xFFFF0000,  // red
		0xFF0000FF,  // blue
		0xFF000000,  // black
	};
	
	const int bar_width = w / 8;
	
	uint32_t* line = (uint32_t*) base;
	
	uint32_t* p = line;
	
	for ( int i = 0;  i < 8;  ++i )
	{
		p = fill( p, bar_width, colors[ i ] );
	}
	
	for ( int j = 1;  j < h;  ++j )
	{
		base += map.get_stride();
		
		memcpy( base, line, w * sizeof (uint32_t) );
	}
	
	return 0;
}
