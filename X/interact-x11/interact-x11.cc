/*
	interact-x11.cc
	---------------
*/

// X11
#include <X11/Xlib.h>
#include <X11/Xutil.h>

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <errno.h>
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

// poseven
#include "poseven/types/thread.hh"

// interact-x11
#include "splode.hh"


#define PROGRAM "interact-x11"

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define USAGE  "usage: " PROGRAM "--raster <raster-path>\n" \
"       where raster-path is a raster file\n"

#define ERROR( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )

namespace p7 = poseven;


enum
{
	Opt_title   = 't',
	Opt_magnify = 'x',
	
	Opt_last_byte = 255,
	
	Opt_feed,
	Opt_raster,
};

static command::option options[] =
{
	{ "feed",    Opt_feed,    command::Param_required },
	{ "raster",  Opt_raster,  command::Param_required },
	{ "title",   Opt_title,   command::Param_required },
	{ "magnify", Opt_magnify, command::Param_required },
	{ NULL }
};

static const char* raster_path;

static raster::raster_load loaded_raster;

static int exit_pipe_fds[ 2 ];


static
void report_error( const char* path, uint32_t err )
{
	more::perror( PROGRAM, path, err );
}

static
raster::sync_relay* open_raster( const char* path )
{
	int raster_fd = open( path, O_RDWR );
	
	if ( raster_fd < 0 )
	{
		report_error( path, errno );
		
		exit( 1 );
	}
	
	using namespace raster;
	
	loaded_raster = play_raster( raster_fd );
	
	if ( loaded_raster.addr == NULL )
	{
		report_error( path, errno );
		
		exit( 1 );
	}
	
	close( raster_fd );
	
	raster_note* sync = find_note( *loaded_raster.meta, Note_sync );
	
	if ( ! is_valid_sync( sync ) )
	{
		report_error( path, ENOSYS );
		
		exit( 3 );
	}
	
	return (sync_relay*) data( sync );
}

static p7::thread raster_update_thread;
static p7::mutex  x11_mutex;

static Display* display;
static Window   window;
static GC       gc;
static XImage*  image;

static
void update_image( unsigned width, unsigned height )
{
	XPutImage( display, window, gc, image, 0,0, 0,0, width, height );
}

static
void update_loop( raster::sync_relay*  sync,
                  unsigned             width,
                  unsigned             height,
                  unsigned             stride )
{
	const char* update_fifo = getenv( "GRAPHICS_UPDATE_SIGNAL_FIFO" );
	
	const raster::raster_desc& desc = loaded_raster.meta->desc;
	
	const size_t image_size = height * stride;
	
	uint32_t seed = 0;
	
	while ( sync->status == raster::Sync_ready )
	{
		while ( seed == sync->seed )
		{
			if ( update_fifo )
			{
				close( open( update_fifo, O_WRONLY ) );
			}
			else
			{
				raster::wait( *sync );
			}
			
			p7::thread::testcancel();
		}
		
		seed = sync->seed;
		
		p7::lock lock( x11_mutex );
		
		image->data = (char*) loaded_raster.addr + desc.frame * image_size;
		
		update_image( width, height );
		
		XFlush( display );
	}
}

static
void wait_for_fd( int fd )
{
	const int exit_fd = exit_pipe_fds[ 0 ];
	
	fd_set readfds;
	FD_ZERO( &readfds );
	
	FD_SET( fd,      &readfds );
	FD_SET( exit_fd, &readfds );
	
	const int max_fd = fd > exit_fd ? fd : exit_fd;
	
retry:
	
	int selected = select( max_fd + 1, &readfds, NULL, NULL, NULL );
	
	if ( selected < 0  &&  errno == EINTR )
	{
		goto retry;
	}
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
			case Opt_feed:
				break;
			
			case Opt_raster:
				raster_path = global_result.param;
				break;
			
			case Opt_title:
				break;
			
			case Opt_magnify:
				break;
			
			default:
				break;
		}
	}
	
	return argv;
}

static
void* raster_update_start( void* arg )
{
	raster::sync_relay* sync = (raster::sync_relay*) arg;
	
	const raster::raster_desc& desc = loaded_raster.meta->desc;
	
	update_loop( sync, desc.width, desc.height, desc.stride );
	
	close( exit_pipe_fds[ 1 ] );
	
	return NULL;
}

int main( int argc, char** argv )
{
	if ( argc == 0 )
	{
		return 0;
	}
	
	char* const* args = get_options( argv );
	
	int argn = argc - (args - argv);
	
	if ( raster_path == NULL )
	{
		write( STDERR_FILENO, STR_LEN( USAGE ) );
		return 2;
	}
	
	if ( const char* var = getenv( "INTERACT_SWAP_CMD_OPT" ) )
	{
		swap_Command_and_Option = atoi( var );
	}
	
	raster::sync_relay* sync = open_raster( raster_path );
	
	const raster::raster_desc& desc = loaded_raster.meta->desc;
	
	unsigned width  = desc.width;
	unsigned height = desc.height;
	unsigned depth  = desc.weight;
	unsigned stride = desc.stride;
	
	if ( depth != 1 )
	{
		ERROR( "only 1-bit-deep rasters supported" );
		return 1;
	}
	
	pipe( exit_pipe_fds );
	
	display = XOpenDisplay( NULL );
	
	if ( display == NULL )
	{
		exit( 1 );
	}
	
	int s = DefaultScreen( display );
	
	gc = DefaultGC( display, s );
	
	Visual* const visual = DefaultVisual( display, s );
	
	image = XCreateImage( display, visual, depth, XYBitmap, 0,
	                      (char*) loaded_raster.addr, width, height, 16,
	                      stride );
	
	/*
		Specify big-endian bit order so that XPutImage() will automatically
		perform in-byte bit reversal if the server requires the other order.
		Also specify big-endian byte order, since we told XCreateImage() to
		operate in units of 16 bits.
	*/
	
	image->bitmap_bit_order = MSBFirst;
	image->byte_order       = MSBFirst;
	
	Status status = XInitImage( image );
	
	// Window managers override this anyway
	const int window_x = 0;
	const int window_y = 0;
	
	const unsigned long black = BlackPixel( display, s );
	const unsigned long white = WhitePixel( display, s );
	
	window = XCreateSimpleWindow( display,
	                              RootWindow( display, s ),
	                              window_x, window_y, width, height,
	                              1,
	                              black, white );
	
	long eventMask = ExposureMask
	               | VisibilityChangeMask
	               | PointerMotionMask
	               | ButtonMotionMask
	               | ButtonPressMask
	               | ButtonReleaseMask
	               | KeyPressMask
	               | KeyReleaseMask;
	
	XSelectInput( display, window, eventMask );
	
	XMapWindow( display, window );
	
	Window root;
	Window child;
	int screen_x;
	int screen_y;
	int x;
	int y;
	unsigned mask;
	
	XFlush( display );
	
	raster_update_thread.create( &raster_update_start, sync );
	
	int x11_fd = ConnectionNumber( display );
	
	/*
		Send a NUL byte.  This may be intercepted by exhibit to indicate
		readiness, or passed on to the raster author as a null SPIEL message.
	*/
	
	write( STDOUT_FILENO, "", 1 );
	
	for ( ;; )
	{
		wait_for_fd( x11_fd );
		
		if ( raster_update_thread.terminated() )
		{
			break;
		}
		
		p7::lock lock( x11_mutex );
		
		XEvent event;
		
		do
		{
			XNextEvent( display, &event );
			
			switch ( event.type )
			{
				case VisibilityNotify:
					if ( XQueryPointer( display, window,
					                    &root, &child,
					                    &screen_x, &screen_y,
					                    &x, &y, &mask ) )
					{
						if ( x >= 0  &&  y >= 0  &&  x < width  && y < height )
						{
							send_cursor_location( x, y );
						}
					}
					break;
				
				case Expose:
					update_image( width, height );
					break;
				
				case MotionNotify:
					send_motion_event( event.xmotion );
					break;
				
				case ButtonPress:
				case ButtonRelease:
					send_motion_event( event.xmotion );
					send_button_event( event.xbutton );
					break;
				
				case KeyPress:
				case KeyRelease:
					send_key_event( event.xkey );
					break;
				
				default:
					break;
			}
		}
		while ( XPending( display ) );
	}
	
	raster_update_thread.cancel( &sync->cond );
	raster_update_thread.join();
	
	image->data = NULL;
	XDestroyImage( image );
	XDestroyWindow( display, window );
	
	XCloseDisplay( display );
	
	return 0;
}
