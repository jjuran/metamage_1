/*
	nyancat.cc
	----------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>

// Standard C
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

// more-posix
#include "more/perror.hh"

// command
#include "command/get_option.hh"

// gear
#include "gear/parse_decimal.hh"

// raster
#include "raster/load.hh"
#include "raster/relay.hh"

// nyancatlib
#include "nyancat/graphics.hh"

// nyancat
#include "bitmap.hh"


#define PROGRAM  "nyancat"

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define ERROR( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )

using nyancat::bitmap;
using nyancat::n_frames;

using raster::raster_desc;
using raster::raster_load;
using raster::sync_relay;


enum
{
	Opt_file = 'f',
	
	Opt_width  = 'w',
	Opt_height = 'h',
	
	Opt_interval = 'i',
};

static command::option options[] =
{
	{ "file", Opt_file, command::Param_required },
	{ "width", Opt_width, command::Param_required },
	{ "height", Opt_height, command::Param_required },
	{ "interval", Opt_interval, command::Param_required },
	{ NULL }
};

static uint32_t width  = 70;
static uint32_t height = 72;

const uint8_t weight = 32;

static const char* graphics_file;

static int interval;

static bool paused;

static raster_load loaded_raster;


static inline
void report_error( const char* path, uint32_t err )
{
	more::perror( PROGRAM, path, err );
}

static inline
uint32_t make_stride( uint32_t width, int weight )
{
	if ( weight > 8 )
	{
		return width * (weight / 8);
	}
	
	if ( weight < 8 )
	{
		int pixels_per_byte = 8 / weight;
		
		width += pixels_per_byte - 1;
		width /= pixels_per_byte;
	}
	
	// width is now in bytes
	
	const int alignment = 2;
	const int mask = alignment - 1;
	
	return width + mask & ~mask;
}

static
void close_without_errno( int fd )
{
	const int saved_errno = errno;
	
	close( fd );
	
	errno = saved_errno;
}

static
int create_raster( const char* path )
{
	using namespace raster;
	
	int fd = open( path, O_RDWR );
	
	if ( fd < 0 )
	{
		return errno;
	}
	
	loaded_raster = play_raster( fd );
	
	close_without_errno( fd );
	
	if ( loaded_raster.addr == 0 )  // NULL
	{
		return errno;
	}
	
	return 0;
}

static inline
void fill( uint32_t* begin, uint32_t* end, uint32_t x )
{
	while ( begin < end )
	{
		*begin++ = x;
	}
}

static
void fill_frame( bitmap& bits )
{
	uint32_t* dst = bits.begin();
	uint32_t* end = bits.end();
	
	const uint32_t dusk = 0xFF224477;
	
	fill( dst, end, dusk );
}

static
timeval timeval_from_us( unsigned long long microseconds )
{
	timeval tv = { 0, (suseconds_t) microseconds };
	
	if ( microseconds > 999999 )
	{
		tv.tv_sec  = microseconds / 1000000;
		tv.tv_usec = microseconds % 1000000;
	}
	
	return tv;
}

static
timeval timeval_diff( const timeval& a, const timeval& b )
{
	const bool borrow = a.tv_usec < b.tv_usec;
	
	long sec = a.tv_sec  - b.tv_sec  - borrow;
	int usec = a.tv_usec - b.tv_usec + borrow * 1000000;
	
	if ( sec < 0 )
	{
		sec  = 0;
		usec = 0;
	}
	
	const timeval result = { sec, usec };
	
	return result;
}

static inline
timeval operator-( const timeval& a, const timeval& b )
{
	return timeval_diff( a, b );
}

static
bool wait_for_fd( int fd, timeval* timeout )
{
	fd_set readfds;
	FD_ZERO( &readfds );
	FD_SET( fd, &readfds );
	
	const int max_fd = fd;
	
	timeval then, now;
	gettimeofday( &then, NULL );
	
	int selected = select( max_fd + 1, &readfds, NULL, NULL, timeout );
	
	if ( selected  &&  timeout )
	{
		gettimeofday( &now, NULL );
		
		*timeout = *timeout - (now - then);
	}
	
	return selected > 0;
}

static inline
ssize_t direct_read( int fd, unsigned char* buffer, size_t n )
{
	return read( fd, buffer, n );
}

static inline
ssize_t normal_read( int fd, unsigned char* buffer, size_t n )
{
	ssize_t result = read( fd, buffer, sizeof (char) );
	
	if ( result > 0 )
	{
		result = read( fd, buffer + 1, buffer[ 0 ] );
		
		if ( result >= 0 )
		{
			++result;
		}
	}
	
	return result;
}

static inline
ssize_t read( int fd, unsigned char* buffer, size_t n, bool direct )
{
	return direct ? direct_read( fd, buffer, n )
	              : normal_read( fd, buffer, n );
}

static
uint8_t read_event( int fd, unsigned char buffer[ 256 ] )
{
	/*
		Reading 0 bytes from a normal stream yields 0, but an eventtap stream
		will fail with EINVAL.
	*/
	
	const bool direct = read( fd, buffer, 0 );
	
	const ssize_t n_read = read( fd, buffer, 256, direct );
	
	if ( n_read == 0 )
	{
		return n_read;
	}
	
	const ssize_t len = buffer[ 0 ];
	
	if ( 1 + len != n_read )
	{
		write( STDERR_FILENO, STR_LEN( PROGRAM ": short read\n" ) );
		exit( 1 );
	}
	
	return n_read;
}

static
uint16_t get_keydown( const unsigned char buffer[ 256 ] )
{
	const ssize_t len = buffer[ 0 ];
	
	switch ( len )
	{
		case 1:
			return buffer[ 1 ];  // ASCII insertion
		
		case 3:
			if ( buffer[ 2 ] < 2 )
			{
				return ' ';  // click or down-press, simulate Space
			}
			break;
		
		case 4:
			if ( buffer[ 3 ] != 2 )
			{
				const uint16_t modes = buffer[ 2 ] << 8;
				const uint16_t ascii = buffer[ 1 ];
				
				return modes | ascii;  // key click, down-press, or repeat
			}
			break;
		
		default:
			break;
	}
	
	return 0;
}

static
char* const* get_options( char** argv )
{
	int opt;
	
	++argv;  // skip arg 0
	
	while ( (opt = command::get_option( (char* const**) &argv, options )) > 0 )
	{
		using command::global_result;
		
		switch ( opt )
		{
			case Opt_file:
				graphics_file = global_result.param;
				break;
			
			case Opt_width:
				width = gear::parse_unsigned_decimal( global_result.param );
				break;
			
			case Opt_height:
				height = gear::parse_unsigned_decimal( global_result.param );
				break;
			
			case Opt_interval:
				interval = gear::parse_unsigned_decimal( global_result.param );
				break;
			
			default:
				break;
		}
	}
	
	return argv;
}

int main( int argc, char** argv )
{
	if ( argc == 0 )
	{
		return 0;  // whatever
	}
	
	char* const* args = get_options( argv );
	
	int argn = argc - (args - argv);
	
	if ( graphics_file == NULL )
	{
		ERROR( "-f argument required" );
		return 2;
	}
	
	if ( width == 0  ||  height == 0 )
	{
		ERROR( "width and height must be non-zero" );
		return 2;
	}
	
	int err = create_raster( graphics_file );
	
	if ( err )
	{
		report_error( graphics_file, err );
		return 1;
	}
	
	sync_relay* relay = &raster::data< sync_relay >( loaded_raster.meta->note );
	
	if ( interval < 20 )
	{
		interval = 100;
	}
	
	unsigned t = 0;
	
	const raster_desc& desc = loaded_raster.meta->desc;
	
	const uint32_t image_size = desc.height * desc.stride;
	
	bitmap bits( (uint32_t*) loaded_raster.addr, image_size, desc.stride );
	
	draw_frame( bits, 0 );
	
	raster::broadcast( *relay );
	
	int step = 0;
	
	timeval tv = timeval_from_us( interval * 1000 );
	
	while ( true )
	{
		if ( wait_for_fd( STDIN_FILENO, paused ? NULL : &tv ) )
		{
			unsigned char buffer[ 256 ];
			
			if ( read_event( STDIN_FILENO, buffer ) == 0 )
			{
				break;
			}
			
			uint16_t key = get_keydown( buffer );
			
			step = 0;
			
			switch ( key )
			{
				case 'q':
					goto cleanup;
				
				case ' ':
					paused = ! paused;
					break;
				
				case '\x1c':  // left arrow
					step = -1;
					break;
				
				case '\x1d':  // right arrow
					step = 1;
					break;
				
				case '\x1e':  // up arrow
					interval += 10;
					break;
				
				case '\x1f':  // down arrow
					if ( interval > 20 )
					{
						interval -= 10;
					}
					break;
				
				default:
					continue;
			}
		}
		else
		{
			tv = timeval_from_us( interval * 1000 );
			
			step = ! paused;
		}
		
		int dt = step;
		
		t += dt;
		t %= n_frames;
		
		fill_frame( bits );
		draw_frame( bits, t );
		
		raster::broadcast( *relay );
	}
	
cleanup:
	
	raster::terminate( *relay );
	
	return 0;
}
