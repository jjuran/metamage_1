/*
	splode.cc
	---------
*/

#include "splode.hh"

// Mac OS
#ifndef __EVENTS__
#include <Events.h>
#endif

// POSIX
#include <unistd.h>

// splode
#include "splode/splode.hh"

// ams-common
#include "reactor.hh"

// ams-core
#include "cursor-core.hh"
#include "keycodes.hh"
#include "options.hh"
#include "reactor-core.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)


Byte   MBState : 0x0172;
UInt16 KeyMods : 0x017A;
Point  Mouse   : 0x0830;
Rect   CrsrPin : 0x0834;


bool button_clicked;


static const timeval zero_timeout = { 0, 0 };

static
timeval timeval_from_ticks( unsigned long ticks )
{
	const uint64_t microseconds_per_tick = 1000 * 1000 * 100 / 6015;
	
	const uint64_t dt = ticks * microseconds_per_tick;
	
	const timeval tv = { dt / 1000000, dt % 1000000 };
	
	return tv;
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
UInt16 keymods_from_modifiers_high_byte( uint8_t mod )
{
	UInt16 result = mod;
	
	result = result >> 1 | result << 15;
	
	return result;
}

static
void post_event( const splode::ascii_synth_buffer& buffer )
{
	const UInt32 message = buffer.ascii;
	
	PostEvent( keyDown, message );
	PostEvent( keyUp,   message );
}

static
void post_event( const splode::pointer_event_buffer& buffer )
{
	using namespace splode::modes;
	using namespace splode::pointer;
	using splode::uint8_t;
	
	const uint8_t mode_mask = Command | Shift | Option | Control;
	
	const uint8_t mod = buffer.modes & mode_mask;
	
	KeyMods = keymods_from_modifiers_high_byte( mod );
	
	const uint8_t action = buffer.attrs & action_mask;
	
	if ( action <= 1 )
	{
		button_clicked = true;
	}
	
	if ( action == 0 )
	{
		PostEvent( mouseDown, 0 );
		PostEvent( mouseUp,   0 );
		
		return;
	}
	
	MBState = action == 1 ? 0x00 : 0x80;
	
	const short what = action + (mouseDown - splode::pointer::down);
	
	PostEvent( what, 0 );
}

static
void post_event( const splode::ascii_event_buffer& buffer )
{
	using namespace splode::modes;
	using namespace splode::key;
	using splode::uint8_t;
	
	const uint8_t ascii = buffer.ascii;
	const int8_t  code  = reverse_lookup_general_key[ ascii & 0x7F ];
	
	const UInt32 message = code << 8 | ascii;
	
	const uint8_t mode_mask = Command | Shift | Option | Control;
	const uint8_t attr_mask = Alpha;
	
	const uint8_t mod = (buffer.modes & mode_mask) | (buffer.attrs & attr_mask);
	
	KeyMods = keymods_from_modifiers_high_byte( mod );
	
	const uint8_t action = buffer.attrs & action_mask;
	
	if ( action == 0 )
	{
		PostEvent( keyDown, message );
		PostEvent( keyUp,   message );
		
		return;
	}
	
	const short what = action + (keyDown - splode::key::down);
	
	PostEvent( what, message );
}

static
void SetMouse( short x, short y )
{
	Mouse.h = x;
	Mouse.v = y;
	
	const short width  = CrsrPin.right;
	const short height = CrsrPin.bottom;
	
	if ( Mouse.h < 0 )
	{
		Mouse.h = 0;
	}
	else if ( Mouse.h >= width )
	{
		Mouse.h = width - 1;
	}
	
	if ( Mouse.v < 0 )
	{
		Mouse.v = 0;
	}
	else if ( Mouse.v >= height )
	{
		Mouse.v = height - 1;
	}
	
	update_cursor_location();
}

static inline
void SetMouse( const splode::pointer_movement_buffer& buffer )
{
	SetMouse( Mouse.h + buffer.dx, Mouse.v - buffer.dy );
}

static inline
void SetMouse( const splode::pointer_location_buffer& buffer )
{
	SetMouse( buffer.x, buffer.y );
}

static
void queue_event( int fd )
{
	unsigned char buffer[ 256 ];
	
	/*
		Reading 0 bytes from a normal stream yields 0, but an eventtap stream
		will fail with EINVAL.
	*/
	
	const bool direct = read( fd, buffer, 0 );
	
	const ssize_t n_read = read( fd, buffer, sizeof buffer, direct );
	
	const ssize_t len = buffer[ 0 ];
	
	if ( 1 + len != n_read )
	{
		write( STDERR_FILENO, STR_LEN( "ams-core: SPIEL protocol error\n" ) );
		
		_exit( 1 );
	}
	
	switch ( len )
	{
		default:
		case 0:
			using splode::null_message_buffer;
			break;
		
		case 1:
			post_event( *(splode::ascii_synth_buffer*) buffer );
			break;
		
		case 2:
			using splode::pointer_movement_buffer;
			
			SetMouse( *(pointer_movement_buffer*) buffer );
			break;
		
		case 3:
			post_event( *(splode::pointer_event_buffer*) buffer );
			break;
		
		case 4:
			post_event( *(splode::ascii_event_buffer*) buffer );
			break;
		
		case 5:
			using splode::pointer_location_buffer;
			
			SetMouse( *(pointer_location_buffer*) buffer );
			break;
	}
}

static char reentered_flag;

static inline
asm char reentered()
{
	TAS.B    reentered_flag
	SNE.B    D0
}

static reactor_node splode_node;

static
void splode_ready( reactor_node* )
{
	queue_event( events_fd );
}

static
void reactor_insert()
{
	splode_node.fd    = events_fd;
	splode_node.ready = &splode_ready;
	
	the_reactor_core.insert( &splode_node );
}

static
void wait_for_user_input( const timeval& timeout )
{
	static timeval wait_timeout;
	
	if ( reentered() )
	{
		return;
	}
	
	wait_timeout = timeout;
	
	static int inserted = (reactor_insert(), 0);
	
	while ( reactor_wait( &wait_timeout ) )
	{
		wait_timeout = zero_timeout;
		
		// already handled
	}
	
	reentered_flag = 0;
}

void wait_for_user_input( unsigned long ticks )
{
	wait_for_user_input( timeval_from_ticks( ticks ) );
}

void poll_user_input()
{
	wait_for_user_input( zero_timeout );
}
