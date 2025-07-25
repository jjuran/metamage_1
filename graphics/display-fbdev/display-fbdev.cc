/*
	display-fbdev.cc
	----------------
*/

// Linux
#include <linux/fb.h>
#include <linux/kd.h>

// POSIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>

// Standard C
#include <errno.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// config
#include "config/install.h"

// more-posix
#include "more/perror.hh"

// command
#include "command/get_option.hh"

// cursorlib
#include "plot_cursor.hh"

// v68k-cursor
#include "cursor/cursor.hh"

// shared_cursor
#include "shared_cursor/cursor_file.hh"

// rasterlib
#include "raster/clut.hh"
#include "raster/clut_detail.hh"
#include "raster/load.hh"
#include "raster/relay_detail.hh"
#include "raster/skif.hh"
#include "raster/sync.hh"

// display-fbdev
#include "fb.hh"
#include "transcode.hh"


#define KDMODE  CONFIG_INSTALL_PREFIX "/lib/metamage/kdmode"

#define PROGRAM  "display-fbdev"

#define DEFAULT_FB_PATH  "/dev/fb0"

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define USAGE  "usage: " PROGRAM " <screen-path>\n" \
"       where screen-path is a raster file\n"

#define SUDO_NEEDED "Note: root privileges required to switch console modes"

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )


/*
	Theoretically, 8-bit should work, but we've had mixed success with it on
	Raspberry Pi.  16-bit works fine on RPi, but not on the PC graphics chips
	I've tried.  This may need to become a run-time switch in the future.
	
	For now, just use 32-bit bilevel pixels in all cases.  It works fine on
	the Raspberry Pi, memory's not an issue, and it opens up opportunities
	for post-processing, including mixing with other graphics sources.
	
	Update:  Raspberry OS "Bullseye" has a new graphics driver that prevents
	switching to 32-bit depth.  So, (a) we need code to handle 16-bit depth,
	(b) we won't know until run time if we need to use it, so (c) we may as
	well use it opportunistically if we're already at 16-bit depth and the
	image can be displayed losslessly.  TODO:  For 32-bit images, we should
	attempt the depth switch but fall back to downsampling if it fails.
*/

enum
{
	Opt_watch   = 'W',
	Opt_exhibit = 'X',  // exhibition mode, don't catch SIGINT
	Opt_gfxmode = 'g',
	Opt_title   = 't',
	Opt_wait    = 'w',
	Opt_magnify = 'x',  // allowed values are 1 (no-op) and 2
	
	Opt_graphics_mode = Opt_gfxmode,
};

static command::option options[] =
{
	{ "exhibit-mode",  Opt_exhibit },
	{ "graphics-mode", Opt_graphics_mode },
	
	{ "magnify", Opt_magnify, command::Param_required },
	{ "title",   Opt_title,   command::Param_required },
	{ "wait",    Opt_wait  },
	{ "watch",   Opt_watch },
	{ NULL }
};

static int  the_corner;
static bool gfx_mode;
static bool doubling;
static bool waiting;
static bool watching;
static bool exhibiting;

static size_t reflection_height;
static size_t output_byte_width;

static const char* cursor_path = getenv( "DISPLAY_CURSOR" );

static raster::raster_load loaded_raster;

using v68k::cursor::shared_cursor_state;

static const shared_cursor_state* cursor_state;


static inline
size_t min( size_t a, size_t b )
{
	return b < a ? b : a;
}

static
void report_error( const char* path, uint32_t err )
{
	more::perror( PROGRAM, path, err );
}

static
const raster::sync_relay* open_raster( const char* path )
{
	int raster_fd = open( path, O_RDONLY );
	
	if ( raster_fd < 0 )
	{
		report_error( path, errno );
		
		exit( 1 );
	}
	
	using namespace raster;
	
	loaded_raster = open_raster( raster_fd, watching );
	
	if ( loaded_raster.addr == NULL )
	{
		report_error( path, errno );
		
		exit( 1 );
	}
	
	close( raster_fd );
	
	if ( watching )
	{
		const sync_relay* sync = find_sync( &loaded_raster.meta->note );
		
		if ( ! sync )
		{
			report_error( path, ENOSYS );
			exit( 3 );
		}
		
		return sync;
	}
	
	return NULL;
}

typedef void (*draw_proc)( const uint8_t* src, uint8_t* dst, int width );

enum byte_remapping
{
	Byte_remap_none,
	Byte_remap_swap,
	Byte_remap_rgba,
	Byte_remap_both = Byte_remap_swap | Byte_remap_rgba,
};

using namespace raster;

static
draw_proc select_16b_draw_proc( const raster_desc& desc, byte_remapping remap )
{
	typedef uint16_t bilevel_pixel_t;
	
	#define CASE_MONOCHROME( bpp )  \
		case bpp:  \
			return doubling ? &lookup_N_to_direct< bilevel_pixel_t, bpp, 2 >  \
			                : &lookup_N_to_direct< bilevel_pixel_t, bpp, 1 >
	
	uint8_t last_pixel;
	
	switch ( desc.weight )
	{
		CASE_MONOCHROME( 1 );
		CASE_MONOCHROME( 2 );
		CASE_MONOCHROME( 4 );
		CASE_MONOCHROME( 8 );
		
		case 16:
			/*
				Recognizing that display-fbdev is specific to Linux,
				we're assuming that the screen is RGB 5/6/5 in all cases.
				
				For xRGB 1/5/5/5 images (from Mac OS), we're additionally
				assuming that the host screen is little-endian.
				
				For RGB 5/6/5 images (e.g. from Linux), we assume that
				the image is the same endianness as the host screen.
			*/
			
			if ( bool swapped = remap & 1 )
			{
				if ( ! is_16bit_565( desc ) )
				{
					return doubling ? &rgb555_BE_to_565_LE_2x
					                : &rgb555_BE_to_565_LE;
				}
			}
			
			return is_16bit_565( desc ) ? doubling ? &copy_16_2x
			                                       : &copy_16
			                            : doubling ? &rgb555_LE_to_565_LE_2x
			                                       : &rgb555_LE_to_565_LE;
		
		default:
			return NULL;
	}
	
	#undef CASE_MONOCHROME
}

static
draw_proc select_32b_draw_proc( const raster_desc& desc, byte_remapping remap )
{
	typedef uint32_t bilevel_pixel_t;
	
	#define CASE_MONOCHROME( bpp )  \
		case bpp:  \
			return doubling ? &lookup_N_to_direct< bilevel_pixel_t, bpp, 2 >  \
			                : &lookup_N_to_direct< bilevel_pixel_t, bpp, 1 >
	
	uint8_t last_pixel;
	
	switch ( desc.weight )
	{
		CASE_MONOCHROME( 1 );
		CASE_MONOCHROME( 2 );
		CASE_MONOCHROME( 4 );
		CASE_MONOCHROME( 8 );
		
		case 16:
			/*
				We're assuming that RGB 5/6/5 is little-endian.
			*/
			
			if ( bool swapped = remap & 1 )
			{
				if ( ! is_16bit_565( desc ) )
				{
					return doubling ? &rgb555_BE_to_32_2x
					                : &rgb555_BE_to_32;
				}
			}
			
			return is_16bit_565( desc ) ? doubling ? &rgb565_LE_to_32_2x
			                                       : &rgb565_LE_to_32
			                            : doubling ? &rgb555_LE_to_32_2x
			                                       : &rgb555_LE_to_32;
		
		case 32:
			last_pixel = (uint8_t) desc.layout.per_pixel;
			
			if ( remap & 0x2 )
			{
				bool swapped = desc.magic ? last_pixel == 0x01 : remap & 1;
				
				return swapped ? doubling ? &both_32_2x : &both_32
				               : doubling ? &rgba_32_2x : &rgba_32;
			}
			else
			{
				bool swapped = desc.magic ? last_pixel != 0x03 : remap & 1;
				
				return swapped ? doubling ? &swap_32_2x : &swap_32
				               : doubling ? &copy_32_2x : &copy_32;
			}
		
		default:
			return NULL;
	}
	
	#undef CASE_MONOCHROME
}

static inline
bool is_rgbx_or_rgba( const raster_desc& desc )
{
	if ( desc.magic == kSKIFFileType )
	{
		return (desc.layout.per_pixel | 0x00000004) == 0x01020304  ||
		       (desc.layout.per_pixel | 0x04000000) == 0x04030201;
	}
	
	return (desc.model | (Model_RGBx ^ Model_RGBA)) == Model_RGBA;
}

static
bool is_byte_swapped( const raster_load& raster )
{
	const uint8_t* addr = (uint8_t*) raster.addr;
	
	const uint32_t footer_size = *(uint32_t*) (addr + raster.size - 4);
	
	return footer_size > 0xFFFF;
}

static
void memcpy_fixmul( uint8_t* p, const uint8_t* q, size_t n, int fraction )
{
	while ( n-- )
	{
		*p++ = *q++ * fraction / 256;
	}
}

static
void blit( const uint8_t*  src,
           size_t          src_stride,
           uint8_t*        dst,
           size_t          dst_stride,
           size_t          width,
           size_t          height,
           draw_proc       draw )
{
	const size_t source_rows_reflection_height = reflection_height >> doubling;
	
	while ( height > source_rows_reflection_height )
	{
		if ( doubling )
		{
			draw( src, dst, width );
			
			dst += dst_stride;
		}
		
		draw( src, dst, width );
		
		src += src_stride;
		dst += dst_stride;
		
		--height;
	}
	
	if ( reflection_height == 0 )
	{
		return;
	}
	
	const int denom = reflection_height * 4;
	
	size_t n = 0;
	
	uint8_t* tmp = (uint8_t*) alloca( output_byte_width );
	
	memset( tmp, '\0', output_byte_width );
	
	uint8_t* fxp = dst + 2 * reflection_height * dst_stride;
	
	while ( height-- > 0 )
	{
		draw( src, tmp, width );
		
		src += src_stride;
		
		for ( int i = 0;  i <= doubling;  ++i )
		{
			memcpy( dst, tmp, output_byte_width );
			
			dst += dst_stride;
			fxp -= dst_stride;
			
			const int fraction = ++n * 256 / denom;
			
			memcpy_fixmul( fxp, tmp, output_byte_width, fraction );
		}
	}
}

static
void draw_hwcursor( uint8_t* base, size_t stride, size_t width, size_t height )
{
	short v = cursor_state->pointer[ 0 ];
	short h = cursor_state->pointer[ 1 ];
	
	v -= cursor_state->hotspot[ 0 ];
	h -= cursor_state->hotspot[ 1 ];
	
	short top    = v;
	short bottom = v + 16;
	
	if ( top < 0 )
	{
		if ( bottom <= 0 )
		{
			return;
		}
		
		top = 0;
	}
	else if ( bottom > height )
	{
		if ( top >= height )
		{
			return;
		}
		
		bottom = height;
	}
	
	short left  = h & ~0xF;
	short right = left + 32;
	
	if ( left < 0 )
	{
		if ( right <= 0 )
		{
			return;
		}
		
		left  = 0;
		right = 32;
	}
	else if ( right > width )
	{
		if ( left >= width )
		{
			return;
		}
		
		right = width;
		left  = right - 32;
	}
	
	uint8_t* addr = base + top * stride + (left >> 3);
	
	short h_trim = 0;
	short v_skip = 0;
	short v_count = bottom - top;
	
	if ( v < 0 )
	{
		v_skip = -v;
	}
	
	if ( h < 0 )
	{
		h_trim = -1;
	}
	else if ( h >= width - 16 )
	{
		h_trim = 1;
		
		addr += 2;
	}
	
	plot_cursor( cursor_state->face,
	             cursor_state->mask,
	             addr,
	             h & 0xF,
	             h_trim,
	             v_skip,
	             v_count,
	             stride );
}

static volatile sig_atomic_t signalled;

static
void update_loop( const raster::sync_relay*  sync,
                  uint8_t*                   dst,
                  size_t                     dst_stride,
                  size_t                     width,
                  size_t                     height,
                  draw_proc                  draw )
{
	const char* update_fifo = getenv( "GRAPHICS_UPDATE_SIGNAL_FIFO" );
	
	const size_t src_stride = loaded_raster.meta->desc.stride;
	const size_t image_size = src_stride * height;
	
	uint8_t* frame_buffer = (uint8_t*) malloc( image_size );
	
	if ( cursor_path )
	{
		cursor_state = open_cursor_file( cursor_path );
	}
	
	uint16_t cursor_seed = 0;
	uint32_t raster_seed = 0;
	
	uint32_t cursor_loc = (uint32_t) -1;
	
	bool hwcursor_visible = false;
	
	while ( sync->status == Sync_ready  &&  ! signalled )
	{
		/*
			If the raster seed has already changed, it indicates
			that the back end produced a second graphics update
			very shortly after the previous one (that we just
			finished working on), whose notification we probably
			missed (since we weren't waiting).  Handle the new
			update immediately (and any others after that, etc.)
			before waiting again.
		*/
		
		if ( sync->seed == raster_seed )
		{
			if ( update_fifo )
			{
				close( open( update_fifo, O_WRONLY ) );
			}
			else
			{
				usleep( 10000 );  // 10ms
			}
			
			if ( ! cursor_state )
			{
				continue;
			}
			
			if ( ! hwcursor_visible  &&  ! cursor_state->visible )
			{
				/*
					The cursor was and remains invisible, so do nothing.
				*/
				
				continue;
			}
			
			if ( hwcursor_visible  &&  cursor_state->visible )
			{
				if ( cursor_loc == *(uint32_t*) cursor_state->pointer )
				{
					if ( cursor_state->seed == cursor_seed )
					{
						/*
							The cursor remains visible, in the same
							location, with the same image.  Do nothing.
						*/
						
						continue;
					}
				}
			}
			
		}
		
		raster_seed = sync->seed;
		
		if ( cursor_state )
		{
			cursor_seed      = cursor_state->seed;
			hwcursor_visible = cursor_state->visible;
		}
		
		const uint8_t* src = (uint8_t*) loaded_raster.addr;
		
		src += loaded_raster.meta->desc.frame * image_size;
		
		if ( frame_buffer )
		{
			/*
				Copy the frame out of shared memory all at once
				to minimize tearing (at least theoretically).
			*/
			
			memcpy( frame_buffer, src, image_size );
			
			src = frame_buffer;
			
			if ( hwcursor_visible )
			{
				cursor_loc = *(uint32_t*) cursor_state->pointer;
				
				draw_hwcursor( frame_buffer, src_stride, width, height );
			}
		}
		
		blit( src, src_stride, dst, dst_stride, width, height, draw );
	}
	
	free( frame_buffer );
}

static
bool should_modeswitch()
{
	const char* display_modeswitch = getenv( "DISPLAY_MODESWITCH" );
	
	if ( ! display_modeswitch  ||  strcmp( display_modeswitch, "always" ) == 0 )
	{
		return true;
	}
	
	if ( strcmp( display_modeswitch, "never" ) != 0 )
	{
		WARN( "invalid DISPLAY_MODESWITCH" );
		exit( 3 );
	}
	
	return false;
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
			case Opt_exhibit:
				exhibiting = true;
				break;
			
			case Opt_graphics_mode:
				gfx_mode = should_modeswitch();
				break;
			
			case Opt_magnify:
				if ( int x = atoi( global_result.param ) )
				{
					if ( x <= 2 )
					{
						if ( x == 2 )
						{
							doubling = true;
						}
						
						break;
					}
				}
				
				WARN( "invalid -x/--magnify parameter" );
				break;
			
			case Opt_wait:
				waiting = true;
				break;
			
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

static
void exec_or_exit( const char* const argv[] )
{
	execvp( *argv, (char**) argv );
	
	const int saved_errno = errno;
	
	report_error( *argv, saved_errno );
	
	_exit( saved_errno == ENOENT ? 127 : 126 );
}

static
bool console_is_known_to_be_in_graphics_mode()
{
	bool result = false;
	
	int tty0_fd = open( "/dev/tty0", O_WRONLY );
	
	if ( tty0_fd >= 0 )
	{
		long mode;
		int nok = ioctl( tty0_fd, KDGETMODE, &mode );
		
		if ( nok == 0  &&  mode == KD_GRAPHICS )
		{
			result = true;
		}
		
		close( tty0_fd );
	}
	
	return result;
}

static
bool is_suid_root( const char* path )
{
	struct stat st;
	int nok = stat( path, &st );
	
	return nok == 0  &&  st.st_uid == 0  &&  st.st_mode & 04000;
}

static inline
bool sudo_needs_passwd()
{
	return system( "sudo -n true" ) != 0;
}

static pid_t kdmode_pid = 0;

static
void launch_coprocess()
{
	if ( console_is_known_to_be_in_graphics_mode() )
	{
		return;  // No coprocess needed
	}
	
	const bool sudo_needed = ! is_suid_root( KDMODE );
	
	if ( sudo_needed  &&  sudo_needs_passwd() )
	{
		WARN( SUDO_NEEDED );
		
		int status = system( "sudo true" );
		
		if ( WIFSIGNALED( status ) )
		{
			exit( 128 + WTERMSIG( status ) );
		}
		else if ( status != 0 )
		{
			exit( WEXITSTATUS( status ) );
		}
	}
	
	int fds[ 2 ];
	
	int nok = socketpair( PF_UNIX, SOCK_STREAM, 0, fds );
	
	if ( nok )
	{
		report_error( "socketpair", errno );
		exit( 1 );
	}
	
	kdmode_pid = fork();
	
	if ( kdmode_pid < 0 )
	{
		report_error( "fork", errno );
		exit( 1 );
	}
	
	if ( kdmode_pid == 0 )
	{
		const char* argv[] = { "sudo", KDMODE, "graphics", NULL };
		
		dup2( fds[ 1 ], STDIN_FILENO  );
		dup2( fds[ 1 ], STDOUT_FILENO );
		
		close( fds[ 0 ] );
		close( fds[ 1 ] );
		
		exec_or_exit( argv + ! sudo_needed );
	}
	
	close( fds[ 1 ] );
	
	/*
		Wait for kdmode to send a byte to indicate it's ready.
	*/
	
	char dummy;
	ssize_t n_read = read( fds[ 0 ], &dummy, sizeof dummy );
	
	if ( n_read < 0 )
	{
		report_error( "kdmode", errno );
		exit( 1 );
	}
	
	if ( n_read == 0 )
	{
		report_error( "kdmode", ENOTCONN );
		exit( 1 );
	}
}

static const raster::sync_relay* raster_sync;

static
void signal_handler( int )
{
	signalled = true;
}

enum display_format
{
	Format_fullscreen = -1,
	Format_theater,
	Format_center,
	Format_corner = Format_center,
};

static display_format the_format;

static inline
bool preserving_console_text()
{
	return the_format > Format_theater;
}

static inline
bool theater_boxed()
{
	return the_format == Format_theater;
}

static inline
bool showing_fullscreen()
{
	return the_format < Format_theater;
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
	
	const char* env_format = getenv( "DISPLAY_FORMAT" );
	
	if ( env_format )
	{
		if ( strcmp( env_format, "fullscreen" ) == 0 )
		{
			the_format = Format_fullscreen;
		}
		else if ( strcmp( env_format, "center" ) == 0 )
		{
			the_format = Format_center;
		}
		else if ( strcmp( env_format, "top-left" ) == 0 )
		{
			the_corner = 1;
			the_format = Format_corner;
		}
		else if ( strcmp( env_format, "top-right" ) == 0 )
		{
			the_corner = 2;
			the_format = Format_corner;
		}
		else if ( strcmp( env_format, "bottom-left" ) == 0 )
		{
			the_corner = 3;
			the_format = Format_corner;
		}
		else if ( strcmp( env_format, "bottom-right" ) == 0 )
		{
			the_corner = 4;
			the_format = Format_corner;
		}
		else if ( strcmp( env_format, "theater" ) != 0 )
		{
			WARN( "invalid DISPLAY_FORMAT" );
			return 3;
		}
	}
	
	const char* raster_path = args[ 0 ];
	
	raster_sync = open_raster( raster_path );
	
	if ( loaded_raster.addr == NULL )
	{
		report_error( raster_path, errno );
		
		exit( 1 );
	}
	
	const raster_desc& desc = loaded_raster.meta->desc;
	
	fb::handle fbh( DEFAULT_FB_PATH );
	
	fb_var_screeninfo var_info = get_var_screeninfo( fbh );
	
	const fb_var_screeninfo old_var_info = var_info;
	
	if ( (watching  ||  waiting)  &&  ! exhibiting )
	{
		struct sigaction action = { 0 };
		
		action.sa_handler = &signal_handler;
		
		sigaction( SIGHUP,  &action, NULL );
		sigaction( SIGINT,  &action, NULL );
		sigaction( SIGTERM, &action, NULL );
	}
	
	if ( desc.width > var_info.xres  ||  desc.height > var_info.yres )
	{
		the_format = Format_fullscreen;
	}
	
	if ( showing_fullscreen() )
	{
		doubling = false;
	}
	
	if ( 2 * desc.width > var_info.xres  ||  2 * desc.height > var_info.yres )
	{
		doubling = false;
	}
	
	const bool changing_depth = desc.weight > var_info.bits_per_pixel;
	
	if ( changing_depth )
	{
		var_info.bits_per_pixel = desc.weight;
		
		if ( preserving_console_text() )
		{
			set_var_screeninfo( fbh, var_info );
		}
	}
	
	if ( gfx_mode )
	{
		launch_coprocess();
	}
	
	if ( showing_fullscreen() )
	{
		var_info.xres = desc.width;
		var_info.yres = desc.height;
		
		var_info.xres_virtual = desc.width;
		var_info.yres_virtual = desc.height;
	}
	
	if ( ! preserving_console_text() )
	{
		set_var_screeninfo( fbh, var_info );
	}
	
	fb_fix_screeninfo fix_info = get_fix_screeninfo( fbh );
	
	const uint8_t* src = (uint8_t*) loaded_raster.addr /*+ desc.image_offset*/;
	
	fb::map map( fbh );
	
	uint8_t* dst = (uint8_t*) map.get_base();
	
	const size_t dst_stride = fix_info.line_length;
	
	const size_t width  = desc.width;
	const size_t height = desc.height;
	const size_t stride = desc.stride;
	
	if ( theater_boxed()  &&  ! changing_depth )
	{
		memset( dst, '\0', fix_info.smem_len );
	}
	
	if ( ! showing_fullscreen() )
	{
		const uint8_t corner_raw = the_corner - 1;  // 0-based, none = 255
		
		const size_t xwidth  = width  << doubling;
		const size_t xheight = height << doubling;
		
		size_t dx = the_corner & 1 ? 0 : var_info.xres - xwidth;
		size_t dy = corner_raw < 2 ? 0 : var_info.yres - xheight;
		
		if ( the_corner == 0 )
		{
			dx /= 2;
			dy /= 2;
		}
		
		dst += dy * dst_stride;
		dst += dx * var_info.bits_per_pixel / 8;
		
		if ( getenv( "DISPLAY_REFLECTION" )  &&  the_corner < 3 )
		{
			if ( const size_t available_rows = var_info.yres - dy - xheight )
			{
				reflection_height = min( xheight / 2, available_rows );
				output_byte_width = xwidth * sizeof (int32_t);  // assume 32-bit
			}
		}
	}
	
	byte_remapping remap = byte_remapping( is_rgbx_or_rgba( desc ) << 1
	                                     | is_byte_swapped( loaded_raster ) );
	
	draw_proc draw = select_32b_draw_proc( desc, remap );
	
	if ( var_info.bits_per_pixel == 16 )
	{
		draw = select_16b_draw_proc( desc, remap );
	}
	
	if ( desc.model == Model_monochrome_paint )
	{
		monochrome_toggle = -1;
	}
	
	if ( desc.model == Model_palette )
	{
		const raster_note* note = find_note( *loaded_raster.meta, Note_clut );
		
		if ( note )
		{
			const clut_data& clut = data< clut_data >( *note );
			
			for ( int i = 0;  i <= clut.max;  ++i )
			{
				const color& c = clut.palette[ i ];
				
				palette_entries[ i ] = 0xFF           << 24
				                     | (c.red   >> 8) << 16
				                     | (c.green >> 8) <<  8
				                     | (c.blue  >> 8) ;
			}
		}
	}
	
	if ( const raster::sync_relay* sync = raster_sync )
	{
		update_loop( sync, dst, dst_stride, width, height, draw );
	}
	
	blit( src, stride, dst, dst_stride, width, height, draw );
	
	if ( waiting )
	{
		char dummy;
		read( STDIN_FILENO, &dummy, sizeof dummy );
	}
	
	if ( showing_fullscreen() )
	{
		set_var_screeninfo( fbh, old_var_info );
	}
	
	/*
		Our exit will close the kdmode control socket, signalling it's time to
		restore the previous console mode.
	*/
	
	return 0;
}
