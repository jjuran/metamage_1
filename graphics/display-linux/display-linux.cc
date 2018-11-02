/*
	display-linux.cc
	----------------
*/

// Linux
#include <linux/fb.h>

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// more-posix
#include "more/perror.hh"

// command
#include "command/get_option.hh"

// raster
#include "raster/load.hh"
#include "raster/relay.hh"
#include "raster/relay_detail.hh"
#include "raster/sync.hh"

// display-linux
#include "fb.hh"


#define PROGRAM  "display-linux"

#define DEFAULT_FB_PATH  "/dev/fb0"

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define USAGE  "usage: " PROGRAM " <screen-path>\n" \
"       where screen-path is a raster file\n"


/*
	Theoretically, 8-bit should work, but we've had mixed success with it on
	Raspberry Pi.  16-bit works fine on RPi, but not on the PC graphics chips
	I've tried.  This may need to become a run-time switch in the future.
*/

#ifdef __arm__
typedef uint16_t bilevel_pixel_t;
#else
typedef uint32_t bilevel_pixel_t;
#endif

enum
{
	Opt_watch   = 'W',
	Opt_title   = 't',
	Opt_magnify = 'x',  // unimplemented, but accepted for compatibility
};

static command::option options[] =
{
	{ "magnify", Opt_magnify, command::Param_required },
	{ "title",   Opt_title,   command::Param_required },
	{ "watch",   Opt_watch },
	{ NULL }
};

static bool watching;

static raster::raster_load loaded_raster;


static
void report_error( const char* path, uint32_t err )
{
	more::perror( PROGRAM, path, err );
}

static
raster::sync_relay* open_raster( const char* path )
{
	const int flags = watching ? O_RDWR : O_RDONLY;
	
	int raster_fd = open( path, flags );
	
	if ( raster_fd < 0 )
	{
		report_error( path, errno );
		
		exit( 1 );
	}
	
	using namespace raster;
	
	loaded_raster = watching ? play_raster( raster_fd )
	                         : load_raster( raster_fd );
	
	if ( loaded_raster.addr == NULL )
	{
		report_error( path, errno );
		
		exit( 1 );
	}
	
	close( raster_fd );
	
	if ( watching )
	{
		raster_note* sync_note = find_note( *loaded_raster.meta, Note_sync );
		
		if ( ! is_valid_sync( sync_note ) )
		{
			report_error( path, ENOSYS );
			exit( 3 );
		}
		
		return (sync_relay*) data( sync_note );
	}
	
	return NULL;
}

typedef void (*draw_proc)( const char* src, char* dst, int width );

template < class UInt >
static
void transcode_1_to_direct( const char* src, char* dst, int width )
{
	UInt* p = (UInt*) dst;
	
	while ( width > 0 )
	{
		uint8_t byte = *src++;
		
		for ( int mask = 1 << 7;  mask != 0;  mask >>= 1 )
		{
			const UInt pixel = byte & mask ? 0x00000000 : 0xFFFFFFFF;
			
			*p++ = pixel;
		}
		
		width -= 8;
	}
}

static
void copy_8( const char* src, char* dst, int width )
{
	memcpy( dst, src, width );
}

static
void copy_16( const char* src, char* dst, int width )
{
	memcpy( dst, src, width * 2 );
}

static
void copy_32( const char* src, char* dst, int width )
{
	memcpy( dst, src, width * 4 );
}

static
void swap_32( const char* src, char* dst, int width )
{
	while ( width > 0 )
	{
		uint8_t a = *src++;
		uint8_t b = *src++;
		uint8_t c = *src++;
		uint8_t d = *src++;
		
		*dst++ = d;
		*dst++ = c;
		*dst++ = b;
		*dst++ = a;
		
		--width;
	}
}

using namespace raster;

static
draw_proc select_draw_proc( const raster_desc& desc, bool swap_bytes )
{
	switch ( desc.weight )
	{
		case 1:
			return &transcode_1_to_direct< bilevel_pixel_t >;
		
		case 16:
			return &copy_16;
		
		case 32:
			return swap_bytes ? &swap_32 : &copy_32;
		
		default:
			return NULL;
	}
}

static
bool is_byte_swapped( const raster_load& raster )
{
	const char* addr = (char*) raster.addr;
	
	const uint32_t footer_size = *(uint32_t*) (addr + raster.size - 4);
	
	return footer_size > 0xFFFF;
}

static
void blit( const char*  src,
           size_t       src_stride,
           char*        dst,
           size_t       dst_stride,
           size_t       width,
           size_t       height,
           draw_proc    draw )
{
	while ( height-- > 0 )
	{
		draw( src, dst, width );
		
		src += src_stride;
		dst += dst_stride;
	}
}

static
void update_loop( raster::sync_relay*  sync,
                  const char*          src,
                  size_t               src_stride,
                  char*                dst,
                  size_t               dst_stride,
                  size_t               width,
                  size_t               height,
                  draw_proc            draw )
{
	uint32_t seed = 0;
	
	while ( sync->status == Sync_ready )
	{
		while ( seed == sync->seed )
		{
			raster::wait( *sync );
		}
		
		seed = sync->seed;
		
		blit( src, src_stride, dst, dst_stride, width, height, draw );
	}
}

static
char* const* get_options( char** argv )
{
	int opt;
	
	++argv;  // skip arg 0
	
	while ( (opt = command::get_option( (char* const**) &argv, options )) > 0 )
	{
		switch ( opt )
		{
			case Opt_watch:
				watching = true;
				break;
			
			case Opt_title:
				// For compatibility with FORGE display -- ignored
			
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
		return 0;
	}
	
	char* const* args = get_options( argv );
	
	int argn = argc - (args - argv);
	
	if ( argn < 1 )
	{
		write( STDERR_FILENO, STR_LEN( USAGE ) );
		return 2;
	}
	
	const char* raster_path = args[ 0 ];
	
	raster::sync_relay* sync = open_raster( raster_path );
	
	if ( loaded_raster.addr == NULL )
	{
		report_error( raster_path, errno );
		
		exit( 1 );
	}
	
	const raster_desc& desc = loaded_raster.meta->desc;
	
	draw_proc draw = select_draw_proc( desc, is_byte_swapped( loaded_raster ) );
	
	fb::handle fbh( DEFAULT_FB_PATH );
	
	fb_var_screeninfo var_info = get_var_screeninfo( fbh );
	
	const char* fullscreen = getenv( "DISPLAY_FULLSCREEN" );
	
	if ( fullscreen )
	{
		var_info.xres = desc.width;
		var_info.yres = desc.height;
		
		var_info.xres_virtual = desc.width;
		var_info.yres_virtual = desc.height;
	}
	
	var_info.bits_per_pixel = desc.weight;
	
	if ( desc.weight == 1 )
	{
		var_info.bits_per_pixel = sizeof (bilevel_pixel_t) * 8;
	}
	
	set_var_screeninfo( fbh, var_info );
	
	fb_fix_screeninfo fix_info = get_fix_screeninfo( fbh );
	
	size_t size = fix_info.smem_len;
	
	const char* src = (char*) loaded_raster.addr /*+ desc.image_offset*/;
	
	fb::map map( fbh );
	
	char* dst = (char*) map.get_base();
	
	const size_t dst_stride = fix_info.line_length;
	
	const size_t width  = desc.width;
	const size_t height = desc.height;
	const size_t stride = desc.stride;
	
	if ( ! fullscreen )
	{
		dst += (var_info.yres - height) / 2 * dst_stride;
		dst += (var_info.xres - width ) / 2 * var_info.bits_per_pixel / 8;
	}
	
	if ( sync )
	{
		update_loop( sync, src, stride, dst, dst_stride, width, height, draw );
	}
	
	blit( src, stride, dst, dst_stride, width, height, draw );
	
	return 0;
}
