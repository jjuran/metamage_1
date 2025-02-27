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

// Standard C
#include <errno.h>
#include <string.h>

// iota
#include "iota/char_types.hh"

// splode
#include "splode/splode.hh"

// log-of-war
#include "logofwar/report.hh"

// ams-common
#include "reactor.hh"

// ams-core
#include "cursor-core.hh"
#include "keycodes.hh"
#include "keytrans.hh"
#include "options.hh"
#include "reactor-core.hh"


#pragma exceptions off


#define STR_LEN( s )  "" s, (sizeof s - 1)


typedef KeyMapByteArray Keys;

Byte   MBState : 0x0172;
Keys   KeyMaps : 0x0174;
UInt16 KeyMods : 0x017A;  // Yes, this is a subfield of KeyMaps.
Point  RawMouse: 0x082C;
Point  Mouse   : 0x0830;
Rect   CrsrPin : 0x0834;


bool button_clicked;


static inline
timeval approximate_timeval_from_ticks( unsigned long ticks )
{
	/*
		The number of ticks per seconds is 60.15, but we'll use 64, or 2^6.
		
		The number of microseconds per tick is (1000 * 1000 * 100 / 6015),
		which is approximately 16625.1.  We'll use 16384, or 2^14.
		
		To convert microseconds to seconds, we need to divide by 1000000.
		We'll use 1048576, or 2^20.
		
		Since we're multiplying dt by 2^14 and then dividing it by 2^20,
		we can instead just divide it by 2^6.  Using a bit shift, of course.
		
		We're also taking (ticks * 2^14) modulo 2^20, i.e. the low 20 bits.
		We already know the low 14 bits will be zero, so we can do the modulo
		first, keeping the low 6 bits.  (Using bit intersection, naturally.)
	*/
	
	/*
	const UInt64 actual_microseconds_per_tick = 1000 * 1000 * 100 / 6015;
	const UInt64 approx_microseconds_per_tick = 1024 * 1024       / 64;
	
	const UInt64 dt = ticks * microseconds_per_tick;
	*/
	
	UInt32 usec = (ticks & (1 << 6) - 1) << 14;
	
	if ( usec >= 1000000 )
	{
		usec = 999999;
	}
	
	const timeval tv = { ticks >> 6, usec };
	
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
UInt16 keymods_from_modifiers_high_byte( Byte mod )
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
	
	const Byte mode_mask = Command | Shift | Option | Control;
	
	const Byte mod = buffer.modes & mode_mask;
	
	KeyMods = keymods_from_modifiers_high_byte( mod );
	
	const Byte action = buffer.attrs & action_mask;
	
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
	using namespace splode::ascii;
	using namespace splode::modes;
	using namespace splode::key;
	
	const Boolean keypad = buffer.attrs & Keypad;
	const Byte    ascii  = buffer.ascii;
	
	const SignedByte code = keypad ? reverse_lookup_from_keypad[ ascii & 0x1F ]
	                               : reverse_lookup_general_key[ ascii & 0x7F ];
	
	const Byte mode_mask = Command | Shift | Option | Control;
	const Byte attr_mask = Alpha;
	
	const Byte modes = buffer.modes & mode_mask;
	
	if ( ascii == Esc  &&  modes == (Command | Option) )
	{
		/*
			Command-Option Esc:  Force Quit.
			
			This can't be sent using a Mac's keyboard since it would be caught
			by the host operating system.  But it can be sent on Linux (if the
			keyboard has been grabbed for our use), which may be necessary if
			the user launched AMS from the console and needs to force an exit
			(since Ctrl-C won't work with the keyboard grabbed).
			
			There's no dialog yet.  It just exits.
		*/
		
		_exit( 128 );
	}
	
	const Byte mod = modes | (buffer.attrs & attr_mask);
	
	KeyMods = keymods_from_modifiers_high_byte( mod );
	
	if ( ascii == '\0' )
	{
		return;  // Don't post events for NUL; just update KeyMods.
	}
	
	Byte xascii = ascii;
	
	if ( mod & Shift  &&  (SignedByte) ascii >= 0 )
	{
		if ( Byte c = keytrans_shift[ ascii ] )
		{
			xascii = c;
		}
	}
	else if ( mod & Alpha  &&  iota::is_lower( ascii ) )
	{
		xascii = iota::to_upper( ascii );
	}
	
	if ( mod & Option  &&  (SignedByte) xascii >= 0 )
	{
		if ( Byte c = keytrans_option[ xascii ] )
		{
			xascii = c;
		}
	}
	
	if ( mod == Control )
	{
		xascii &= 0x1f;
	}
	
	const UInt32 message = code << 8 | xascii;
	
	const Byte action = buffer.attrs & action_mask;
	
	if ( action == 0 )
	{
		PostEvent( keyDown, message );
		PostEvent( keyUp,   message );
		
		return;
	}
	
	if ( code >= 0 )
	{
		if ( action == splode::key::down )
		{
			KeyMaps[ code >> 3 ] |= 1 << (code & 0x07);
		}
		
		if ( action == splode::key::up )
		{
			KeyMaps[ code >> 3 ] &= ~(1 << (code & 0x07));
		}
	}
	
	const short what = action + (keyDown - splode::key::down);
	
	PostEvent( what, message );
}

static
void SetMouse( short x, short y )
{
	RawMouse.h = x;
	RawMouse.v = y;
	
	const short width  = CrsrPin.right;
	const short height = CrsrPin.bottom;
	
	if ( RawMouse.h < 0 )
	{
		RawMouse.h = 0;
	}
	else if ( RawMouse.h >= width )
	{
		RawMouse.h = width - 1;
	}
	
	if ( RawMouse.v < 0 )
	{
		RawMouse.v = 0;
	}
	else if ( RawMouse.v >= height )
	{
		RawMouse.v = height - 1;
	}
	
	Mouse = RawMouse;
	
	update_cursor_location();
}

static inline
void SetMouse( const splode::pointer_movement_buffer& buffer )
{
	SetMouse( RawMouse.h + buffer.dx, RawMouse.v - buffer.dy );
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
		if ( n_read < 0 )
		{
			const char* error = strerror( errno );
			
			FATAL = "ams-core: event stream error: read failed: ", error;
		}
		else if ( n_read == 0 )
		{
			FATAL = "ams-core: event stream error: writer closed";
		}
		else
		{
			const ssize_t n = n_read;
			const ssize_t d = 1 + len;
			
			FATAL = "ams-core: event stream error: only got ", n, "/", d;
		}
		
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
	wait_for_user_input( approximate_timeval_from_ticks( ticks ) );
}

void poll_user_input()
{
	wait_for_user_input( zero_timeout );
}
