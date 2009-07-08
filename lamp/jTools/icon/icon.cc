/*	=======
 *	icon.cc
 *	=======
 */

// Standard C/C++
#include <cstddef>
#include <cstdlib>
#include <cstdio>

// POSIX
#include <signal.h>
#include <unistd.h>

// Lamp
#include "lamp/grafio.h"
#include "lamp/winio.h"

// POSeven
#include "POSeven/functions/ioctl.hh"
#include "POSeven/functions/open.hh"
#include "POSeven/functions/read.hh"


namespace p7 = poseven;


static int Usage()
{
	std::printf( "%s\n", "usage: icon iconfile" );
	
	return EXIT_FAILURE;
}

static volatile sig_atomic_t global_window_changed = 0;

static void sigwinch_handler( int )
{
	global_window_changed = 1;
}


struct icon_data
{
	unsigned char icon[ 128 ];
	unsigned char mask[ 128 ];
};

static icon_data global_icon_data;


static void read_file( const char* pathname )
{
	ssize_t bytes_read = p7::read( p7::open( pathname, p7::o_rdonly ), (char*) &global_icon_data, sizeof global_icon_data );
	
	if ( bytes_read != sizeof global_icon_data )
	{
		std::exit( EXIT_FAILURE );
	}
}

static void create_window( const char* pathname )
{
	short window_size[2] = { 32, 32 };
	
	unsigned short bg_color[3] = { 0xdddd, 0xdddd, 0xdddd };
	
	p7::ioctl( p7::stdin_fileno, GIOCSBGCOLOR, bg_color );
	
	p7::ioctl( p7::stdin_fileno, WIOCSSIZE, &window_size );
	p7::ioctl( p7::stdin_fileno, WIOCSTITLE, pathname );
}

template < int depth >  struct bit_depth_traits;

template <>
struct bit_depth_traits< 8 >
{
	typedef unsigned char pixel_type;
	
	enum { black = 0xff, white = 0x00 };
};

template <>
struct bit_depth_traits< 16 >
{
	typedef unsigned short pixel_type;
	
	enum { black = 0x0000, white = 0x7fff };
};

template <>
struct bit_depth_traits< 32 >
{
	typedef unsigned long pixel_type;
	
	enum { black = 0x00000000, white = 0x00ffffff };
};

template < int depth >
static void draw_icon_bit( unsigned char  bits,
                           unsigned char  mask,
                           char           bit,
                           void*&         dest )
{
	typedef bit_depth_traits< depth > traits;
	
	typedef typename traits::pixel_type pixel_type;
	
	pixel_type*& pixels( *(pixel_type**) &dest );
	
	if ( mask & 1 << bit )
	{
		*pixels = (bits & 1 << bit) ? traits::black : traits::white;
	}
	
	++pixels;
}

template < int depth >
static void draw_icon_byte( const unsigned char*  base,
                            void*&                pixels )
{
	const unsigned char bits = base[   0 ];
	const unsigned char mask = base[ 128 ];
	
	draw_icon_bit< depth >( bits, mask, 7, pixels );
	draw_icon_bit< depth >( bits, mask, 6, pixels );
	draw_icon_bit< depth >( bits, mask, 5, pixels );
	draw_icon_bit< depth >( bits, mask, 4, pixels );
	draw_icon_bit< depth >( bits, mask, 3, pixels );
	draw_icon_bit< depth >( bits, mask, 2, pixels );
	draw_icon_bit< depth >( bits, mask, 1, pixels );
	draw_icon_bit< depth >( bits, mask, 0, pixels );
}

template < int depth >
static void draw_icon_row( const unsigned char*  base,
                           void*                 pixels )
{
	draw_icon_byte< depth >( base++, pixels );
	draw_icon_byte< depth >( base++, pixels );
	draw_icon_byte< depth >( base++, pixels );
	draw_icon_byte< depth >( base++, pixels );
}

static void draw_window()
{
	char* baseAddr = NULL;
	
	unsigned char  depth    = 0;
	unsigned short rowBytes = 0;
	unsigned short nRows    = 0;
	
	p7::ioctl( p7::stdin_fileno, GIOCGDEPTH,    &depth    );
	p7::ioctl( p7::stdin_fileno, GIOCGROWBYTES, &rowBytes );
	p7::ioctl( p7::stdin_fileno, GIOCGNROWS,    &nRows    );
	p7::ioctl( p7::stdin_fileno, GIOCGBASEADDR, &baseAddr );
	
	size_t size = nRows * rowBytes;
	
	typedef void (*that_which_draws_a_row)( const unsigned char* base, void* pixels );
	
	that_which_draws_a_row draw = NULL;
	
	switch ( depth )
	{
		case 8:
			draw = draw_icon_row< 8 >;
			break;
		
		case 16:
			draw = draw_icon_row< 16 >;
			break;
		
		case 32:
			draw = draw_icon_row< 32 >;
			break;
		
		
		default:
			return;
	}
	
	if ( rowBytes < 32 / 8 * depth )
	{
		return;
	}
	
	if ( nRows > 32 )
	{
		nRows = 32;
	}
	
	for ( int row = 0;  row < nRows;  ++row )
	{
		char* rowBase = baseAddr + row * rowBytes;
		
		draw( global_icon_data.icon + row * 32/8, rowBase );
	}
	
	p7::ioctl( p7::stdin_fileno, GIOCFLUSHRECT, NULL );
}

int main( int argc, char const* const argv[] )
{
	const char* pathname = argc > 1 ? argv[1] : "/dev/fd/0";
	
	read_file( pathname );
	
	create_window( pathname );
	
	draw_window();
	
	signal( SIGWINCH, &sigwinch_handler );
	
	while ( true )
	{
		pause();
		
		if ( global_window_changed )
		{
			global_window_changed = 0;
			
			draw_window();
		}
	}
	
	return 0;
}

