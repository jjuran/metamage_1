/*
	display-linux.cc
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

// raster
#include "raster/load.hh"
#include "raster/relay.hh"
#include "raster/relay_detail.hh"
#include "raster/sync.hh"

// display-linux
#include "fb.hh"


#define KDMODE  CONFIG_INSTALL_PREFIX "/lib/metamage/kdmode"

#define PROGRAM  "display-linux"

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
*/

typedef uint32_t bilevel_pixel_t;

enum
{
	Opt_watch   = 'W',
	Opt_exhibit = 'X',  // exhibition mode, don't catch SIGINT
	Opt_gfxmode = 'g',
	Opt_title   = 't',
	Opt_wait    = 'w',
	Opt_magnify = 'x',  // unimplemented, but accepted for compatibility
	
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
static bool waiting;
static bool watching;
static bool exhibiting;

static size_t reflection_height;

static raster::raster_load loaded_raster;


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

typedef void (*draw_proc)( const uint8_t* src, uint8_t* dst, int width );

template < class UInt, int X = 1 >
static
void transcode_1_to_direct( const uint8_t* src, uint8_t* dst, int width )
{
	UInt* p = (UInt*) dst;
	
	while ( width > 0 )
	{
		uint8_t byte = *src++;
		
		for ( int mask = 1 << 7;  mask != 0;  mask >>= 1 )
		{
			const UInt pixel = byte & mask ? 0x00000000 : 0xFFFFFFFF;
			
			for ( int i = 0;  i < X;  ++i )
			{
				*p++ = pixel;
			}
		}
		
		width -= 8;
	}
}

template < class UInt, int X = 1 >
struct pixtet
{
	UInt pixels[ 8 * X ];
};

template < class UInt, int X = 1 >
static
const pixtet< UInt, X >* make_1_to_direct_table()
{
	typedef pixtet< UInt, X > pixel_unit;
	
	enum
	{
		n_octets = 256,
		
		table_size = n_octets * sizeof (pixel_unit),  // 4K or 8K
	};
	
	pixel_unit* table = (pixel_unit*) malloc( table_size );
	
	pixel_unit* p = table;
	
	for ( int i = 0; i < 256;  ++i )
	{
		uint8_t oct = i;
		
		transcode_1_to_direct< UInt, X >( &oct, (uint8_t*) p, 8 );
		
		++p;
	}
	
	return table;
}

template < class UInt, int X >
static
void lookup_1_to_direct( const uint8_t* src, uint8_t* dst, int width )
{
	typedef pixtet< UInt, X > pixel_unit;
	
	static const pixel_unit* table = make_1_to_direct_table< UInt, X >();
	
	pixel_unit* p = (pixel_unit*) dst;
	
	while ( width > 0 )
	{
		*p++ = table[ *src++ ];
		
		width -= 8;
	}
}

static
void copy_8( const uint8_t* src, uint8_t* dst, int width )
{
	memcpy( dst, src, width );
}

static
void copy_16( const uint8_t* src, uint8_t* dst, int width )
{
	memcpy( dst, src, width * 2 );
}

static
void copy_32( const uint8_t* src, uint8_t* dst, int width )
{
	memcpy( dst, src, width * 4 );
}

static
void swap_32( const uint8_t* src, uint8_t* dst, int width )
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
			return &lookup_1_to_direct< bilevel_pixel_t >;
		
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
	while ( height > reflection_height )
	{
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
	
	uint8_t* tmp = (uint8_t*) alloca( dst_stride );
	
	memset( tmp, '\0', dst_stride );
	
	uint8_t* fxp = dst + 2 * reflection_height * dst_stride;
	
	while ( height-- > 0 )
	{
		draw( src, tmp, width );
		
		memcpy( dst, tmp, dst_stride );
		
		const int fraction = ++n * 256 / denom;
		
		src += src_stride;
		dst += dst_stride;
		
		fxp -= dst_stride;
		
		memcpy_fixmul( fxp, tmp, dst_stride, fraction );
	}
}

static volatile sig_atomic_t signalled;

static
void update_loop( raster::sync_relay*  sync,
                  uint8_t*             dst,
                  size_t               dst_stride,
                  size_t               width,
                  size_t               height,
                  draw_proc            draw )
{
	const size_t src_stride = loaded_raster.meta->desc.stride;
	const size_t image_size = src_stride * height;
	
	uint32_t seed = 0;
	
	while ( sync->status == Sync_ready  &&  ! signalled )
	{
		while ( seed == sync->seed )
		{
			raster::wait( *sync );
		}
		
		seed = sync->seed;
		
		const uint8_t* src = (uint8_t*) loaded_raster.addr;
		
		src += loaded_raster.meta->desc.frame * image_size;
		
		blit( src, src_stride, dst, dst_stride, width, height, draw );
	}
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
		switch ( opt )
		{
			case Opt_exhibit:
				exhibiting = true;
				break;
			
			case Opt_graphics_mode:
				gfx_mode = should_modeswitch();
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
	int nok = stat( KDMODE, &st );
	
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

static raster::sync_relay* raster_sync;

static
void signal_handler( int )
{
	signalled = true;
	
	if ( raster_sync )
	{
		raster::broadcast( *raster_sync );
	}
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
	
	draw_proc draw = select_draw_proc( desc, is_byte_swapped( loaded_raster ) );
	
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
	
	uint8_t bpp = desc.weight;
	
	if ( desc.weight == 1 )
	{
		bpp = sizeof (bilevel_pixel_t) * 8;
	}
	
	const bool changing_depth = bpp != var_info.bits_per_pixel;
	
	if ( changing_depth )
	{
		var_info.bits_per_pixel = bpp;
		
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
		
		size_t dx = the_corner & 1 ? 0 : var_info.xres - width;
		size_t dy = corner_raw < 2 ? 0 : var_info.yres - height;
		
		if ( the_corner == 0 )
		{
			dx /= 2;
			dy /= 2;
		}
		
		dst += dy * dst_stride;
		dst += dx * var_info.bits_per_pixel / 8;
		
		if ( getenv( "DISPLAY_REFLECTION" )  &&  the_corner < 3 )
		{
			reflection_height = min( height / 2, var_info.yres - dy - height );
		}
	}
	
	if ( raster::sync_relay* sync = raster_sync )
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
