/*
	vnc-interact.cc
	---------------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

// more-libc
#include "more/string.h"

// more-posix
#include "more/perror.hh"

// transcodex
#include "transcode/8x_1bpp_to_8bpp.hh"

// gear
#include "gear/parse_decimal.hh"

// command
#include "command/get_option.hh"

// splode
#include "splode/splode.hh"

// raster
#include "raster/load.hh"
#include "raster/relay_detail.hh"
#include "raster/sync.hh"

// rfb
#include "rfb/rfb.hh"

// poseven
#include "poseven/types/thread.hh"


#define PROGRAM  "vnc-interact"

#define USAGE  "usage: " PROGRAM "--raster <path> --events-fd <fd>\n"

#define STR_LEN( s )  "" s, (sizeof s - 1)
#define LEN_STR( s )  (sizeof s - 1), "" s

#define PUT( s )  put( STR_LEN( s ) )

#define POLLING_ENSUES  \
	"GRAPHICS_UPDATE_SIGNAL_FIFO unset -- will poll every 10ms instead"

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )

namespace p7 = poseven;

using namespace rfb;


enum
{
	Opt_title   = 't',
	Opt_magnify = 'x',
	
	Opt_last_byte = 255,
	
	Opt_events_fd,
	Opt_raster,
};

static command::option options[] =
{
	{ "events-fd", Opt_events_fd, command::Param_required },
	{ "raster",  Opt_raster,  command::Param_required },
	{ "title",   Opt_title,   command::Param_required },
	{ "magnify", Opt_magnify, command::Param_required },
	{ NULL }
};

static const char* update_fifo = getenv( "GRAPHICS_UPDATE_SIGNAL_FIFO" );

static int events_fd;

static const char* raster_path;
static const char* title;

static raster::raster_load loaded_raster;

static byte* framebuffer;
static size_t fb_len;

static uint16_t width;
static uint16_t height;

static p7::thread raster_update_thread;
static p7::mutex  update_mutex;
static p7::cond   update_cond;

static int update_requested;
static bool updates_ended;

static uint8_t mode_state[ 16 ];

static const uint8_t mode_shift[ 16 ] =
{
	0,
	1,  // Shift L
	1,  // Shift R
	4,  // Control L
	
	4,  // Control R
	0,
	0,
	5,  // Meta L
	
	5,  // Meta R
	2,  // Alt L
	2,  // Alt R
	0,
	
	0,
	0,
	0,
	0,
};

static
void report_error( const char* path, uint32_t err )
{
	more::perror( PROGRAM, path, err );
}

static
ssize_t read_all( int fd, char* p, size_t n )
{
	const size_t m = n;
	
	ssize_t n_read = 1;
	
	while ( n > 0  &&  (n_read = read( fd, p, n )) > 0 )
	{
		p += n_read;
		n -= n_read;
	}
	
	return + n == 0 ? m          // success, return original n
	       : n_read ? n_read     // read() error, return -1
	       : m == n ? 0          // EOF at zero bytes, return 0
	       :          ~(m - n);  // EOF later, return total read as value < -1
}

static inline
ssize_t read_all( int fd, void* p, size_t n )
{
	return read_all( fd, (char*) p, n );
}

static inline
bool put( const void* p, unsigned n )
{
	return write( STDOUT_FILENO, p, n ) == n;
}

template < class T >
static inline
bool put( const T& x )
{
	return put( &x, sizeof x );
}

static inline
bool get( void* p, unsigned n )
{
	ssize_t n_read = read_all( STDIN_FILENO, p, n );
	
	return n_read > 0;
}

template < class T >
static inline
bool get( T& x )
{
	return get( &x, sizeof x );
}

template < class T >
static
bool get( byte type, T& x )
{
	ssize_t n_read = read_all( STDIN_FILENO, (char*) &x + 1, sizeof x - 1 );
	
	if ( n_read <= 0 )
	{
		return false;
	}
	
	x.type = type;
	
	return true;
}

static inline
void clear_framebuffer()
{
	memset( framebuffer, '\0', fb_len );
}

static
void transcode_framebuffer_8x_1_to_8()
{
	using raster::raster_desc;
	
	const raster_desc& desc = loaded_raster.meta->desc;
	const size_t image_size = desc.height * desc.stride;
	
	uint8_t* p = framebuffer;
	uint8_t* q = (uint8_t*) loaded_raster.addr;
	
	q += desc.frame * image_size;
	
	transcode::_8x_1bpp_to_8bpp( q, framebuffer, fb_len / 8u, 0xFF, 0x00 );
}

static
bool send_update()
{
	if ( ! updates_ended )
	{
		transcode_framebuffer_8x_1_to_8();
	}
	
	FramebufferUpdate_header header =
	{
		FramebufferUpdate,
		{ 0 },
		1,
	};
	
	FramebufferUpdate_element element =
	{
		0,
		0,
		width,
		height,
		0,
	};
	
	p7::lock lock( update_mutex );
	
	return
	put( header  )  &&
	put( element )  &&
	put( framebuffer, fb_len );
}

static inline
bool do_SetPixelFormat( const SetPixelFormat_message& msg )
{
	return true;
}

static inline
bool do_SetEncodings( const SetEncodings_header& header, const S_32* encodings )
{
	return true;
}

static
bool do_FramebufferUpdateRequest( const FramebufferUpdateRequest_message& msg )
{
	bool result = true;
	
	if ( ! msg.is_incremental )
	{
		result = send_update();
	}
	
	p7::lock lock( update_mutex );
	
	update_requested = msg.is_incremental;
	
	if ( update_requested )
	{
		update_cond.signal();
	}
	
	return result;
}

static
bool do_KeyEvent( const KeyEvent_message& msg )
{
	const bool down = msg.is_down;
	
	char c = msg.key;
	
	if ( (int16_t) msg.key < 0 )
	{
		switch ( msg.key )
		{
			case 0xffff:
				c = 0x7F;
				break;
			
			case 0xff52:  // Up
			case 0xff53:  // Right
				c ^= 0x01;
				// fall through
			
			case 0xff51:  // Left
			case 0xff54:  // Down
				c -= 0x51 - 0x1C;
				break;
			
			case 0xffe1:
			case 0xffe2:
			case 0xffe3:
			case 0xffe4:
			case 0xffe7:
			case 0xffe8:
			case 0xffe9:
			case 0xffea:
				mode_state[ c & 0xF ] = down << mode_shift[ c & 0xF ];
				c = '\0';
				break;
		}
	}
	
	uint8_t modes = mode_state[ 1 ]
	              | mode_state[ 2 ]
	              | mode_state[ 3 ]
	              | mode_state[ 4 ]
	              | mode_state[ 7 ]
	              | mode_state[ 8 ]
	              | mode_state[ 9 ]
	              | mode_state[ 10 ];
	
	using namespace splode::modes;
	
	// Alt and Meta are always Command and Option in RFB
	
	modes |= !! (modes & Meta) << Option_bit;   // Meta -> Option
	modes |= !! (modes & Alt ) << Command_bit;  // Alt -> Command
	
	const byte attrs = 2 - down;
	
	splode::ascii_event_buffer buffer =
	{
		sizeof buffer - 1,
		(uint8_t) c,
		modes,
		attrs,
	};
		
	if ( events_fd )
	{
		const size_t n = sizeof buffer;
		
		return write( events_fd, &buffer, n ) == n;
	}
	
	return true;
}

static
bool do_PointerEvent( const PointerEvent_message& msg )
{
	const byte mask = msg.button_mask;
	
	using splode::pointer_location_buffer;
	
	pointer_location_buffer& buffer = (pointer_location_buffer&) msg;
	
	if ( PointerEvent != sizeof buffer - 1 )
	{
		buffer.len = sizeof buffer - 1;
	}
	
	buffer.device = 0;
	
	if ( events_fd )
	{
		write( events_fd, &buffer, sizeof buffer );
	}
	
	static byte button_mask;
	
	if ( mask != button_mask )
	{
		button_mask = mask;
		
		const byte attrs = mask ? 1 : 2;
		
		splode::pointer_event_buffer buffer =
		{
			sizeof buffer - 1,
			0,
			attrs,
		};
		
		if ( events_fd )
		{
			const size_t n = sizeof buffer;
			
			return write( events_fd, &buffer, n ) == n;
		}
	}
	
	return true;
}

static
bool handle_event()
{
	byte type;
	if ( ! get( type ) )
	{
		return false;
	}
	
	switch ( type )
	{
		case SetPixelFormat:
		{
			SetPixelFormat_message msg;
			
			return
			get( type, msg )  &&
			do_SetPixelFormat( msg );
			break;
		}
		
		case SetEncodings:
		{
			SetEncodings_header header;
			size_t n;
			void* p;
			
			return
			get( type, header )             &&
			(n = header.count * 4) <= 4096  &&
			(p = alloca( n ))               &&
			get( p, n )                     &&
			do_SetEncodings( header, (S_32*) p );
		}
		
		case FramebufferUpdateRequest:
		{
			FramebufferUpdateRequest_message msg;
			
			return
			get( type, msg )  &&
			do_FramebufferUpdateRequest( msg );
		}
		
		case KeyEvent:
		{
			KeyEvent_message msg;
			
			return
			get( type, msg )  &&
			do_KeyEvent( msg );
		}
		
		case PointerEvent:
		{
			PointerEvent_message msg;
			
			return
			get( type, msg )  &&
			do_PointerEvent( msg );
		}
		
		default:
			return false;
	}
	
	return true;
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
			case Opt_events_fd:
				events_fd = gear::parse_decimal( global_result.param );
				break;
			
			case Opt_raster:
				raster_path = global_result.param;
				break;
			
			case Opt_title:
				title = global_result.param;
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
void wait_for_update_request()
{
	p7::lock lock( update_mutex );
	
	while ( ! update_requested )
	{
		update_cond.wait( lock );
	}
}

static
void update_loop( raster::sync_relay*  sync,
                  uint8_t const*       src,
                  uint8_t*             dst,
                  unsigned             width,
                  unsigned             height,
                  unsigned             stride )
{
	uint32_t seed = 0;
	
	if ( ! update_fifo )
	{
		WARN( POLLING_ENSUES );
	}
	
	while ( sync->status == raster::Sync_ready )
	{
		wait_for_update_request();
		
		while ( seed == sync->seed )
		{
			if ( update_fifo )
			{
				int fd = open( update_fifo, O_WRONLY );
				
				if ( fd < 0 )
				{
					return;
				}
				
				close( fd );
			}
			else
			{
				usleep( 10000 );  // 10ms
			}
			
			p7::thread::testcancel();
		}
		
		seed = sync->seed;
		
		send_update();
	}
}

static
void* raster_update_start( void* arg )
{
	raster::sync_relay* sync = (raster::sync_relay*) arg;
	
	const raster::raster_desc& desc = loaded_raster.meta->desc;
	
	const uint8_t* src = (uint8_t*) loaded_raster.addr;
	
	uint8_t* dst = framebuffer;
	
	update_loop( sync, src, dst, desc.width, desc.height, desc.stride );
	
	updates_ended = true;
	
	wait_for_update_request();
	
	clear_framebuffer();
	
	send_update();
	
	return NULL;
}

static
void sigusr1( int )
{
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
	
	if ( update_fifo )
	{
		signal( SIGUSR1, &sigusr1 );
	}
	
	raster::sync_relay* sync = open_raster( raster_path );
	
	const raster::raster_desc& desc = loaded_raster.meta->desc;
	
	width  = desc.width;
	height = desc.height;
	
	if ( desc.weight != 1 )
	{
		WARN( "only 1-bit-deep rasters supported" );
		return 1;
	}
	
	framebuffer = (byte*) calloc( height, width );
	
	fb_len = height * width;
	
	ProtocolVersion version;
	char shared_flag;
	
	const size_t max_title_len = 127;
	
	if ( title == NULL )
	{
		title = PROGRAM;
	}
	
	size_t title_len = strlen( title );
	
	if ( title_len > max_title_len )
	{
		title_len = max_title_len;
	}
	
	enum
	{
		bpp   = 8,
		depth = 8,
		is_BE = false,  // doesn't matter for 8-bit
		is_TC = true,   // true color, not palette
		
		max_red   = 255,
		max_green = 255,
		max_blue  = 255,
		
		// red/green/blue shift are zero -- the same bits code for all of them
	};
	
	ServerInit_message< max_title_len > server_init =
	{
		width,
		height,
		{ bpp, depth, is_BE, is_TC, max_red, max_green, max_blue },
		title_len,
	};
	
	mempcpy( server_init.name, title, title_len );
	
	const bool inited =
	PUT( RFB_PROTOCOL_VERSION_3_3 )  &&
	get( version )                   &&
	PUT( "\0\0\0\1" )                &&
	get( shared_flag )               &&
	put( &server_init, sizeof (ServerInit_message< 0 >) + title_len );
	
	if ( ! inited )
	{
		return 1;
	}
	
	raster_update_thread.create( &raster_update_start, sync );
	
	while ( handle_event() )
	{
		continue;
	}
	
	/*
		Interrupt the open() of the update FIFO to unblock update_loop().
	*/
	
	kill( 0, SIGUSR1 );
	
	raster_update_thread.cancel( NULL );
	raster_update_thread.join();
	
	return 0;
}
