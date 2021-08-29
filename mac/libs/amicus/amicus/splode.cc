/*
	splode.cc
	---------
*/

#include "amicus/splode.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// POSIX
#include <unistd.h>

// iota
#include "iota/endian.hh"

// splode
#include "splode/splode.hh"

// amicus
#include "amicus/keycodes.hh"


namespace amicus
{

static short last_x;
static short last_y;

void send_mouse_moved_event( int fd, short x, short y )
{
	if ( x == last_x  &&  y == last_y )
	{
		return;
	}
	
	last_x = x;
	last_y = y;
	
	splode::pointer_location_buffer buffer =
	{
		sizeof buffer - 1,
		0,
		iota::big_u16( x ),
		iota::big_u16( y ),
	};
	
	write( fd, &buffer.len, sizeof buffer );
}

void send_mouse_event( int fd, short action, unsigned flags )
{
	using namespace splode::modes;
	
	const uint8_t general_modifiers = flags;
	const uint8_t button_attributes = action;
	
	splode::pointer_event_buffer buffer =
	{
		sizeof buffer - 1,
		general_modifiers,
		button_attributes,
		0,  // unspecified device
	};
	
	write( fd, &buffer.len, sizeof buffer );
}

void send_mouse_event( int fd, const EventRecord& event )
{
	using namespace splode::modes;
	namespace pointer = splode::pointer;
	
	const uint8_t mode_mask = Command | Shift | Option | Control;
	
	const uint8_t general_modifiers = (event.modifiers >> 8) & mode_mask;
	const uint8_t button_attributes = event.what;  // Yes, the codes match.
	
	splode::pointer_event_buffer buffer =
	{
		sizeof buffer - 1,
		general_modifiers,
		button_attributes,
		0,  // unspecified device
	};
	
	write( fd, &buffer.len, sizeof buffer );
}

void send_key_event( int fd, char c, short modes, short attrs )
{
	splode::ascii_event_buffer buffer =
	{
		sizeof buffer - 1,
		c,
		modes,
		attrs,
	};
	
	write( fd, &buffer.len, sizeof buffer );
}

void send_key_event( int fd, const EventRecord& event )
{
	using namespace splode::modes;
	using namespace splode::key;
	
	const int8_t vcode = (event.message >> 8) & 0x7F;
	
	const signed char c = lookup_from_virtual[ vcode ];
	
	if ( c < 0 )
	{
		//throw non_ascii_char();
	}
	
	const uint8_t mode_mask = Command | Shift | Option | Control;
	const uint8_t attr_mask = Alpha;
	
	// 3/4/5 -> 1/2/3
	const uint8_t action = event.what - 2;
	const uint8_t keypad = is_keypad( vcode ) ? Keypad : 0;
	
	splode::ascii_event_buffer buffer =
	{
		sizeof buffer - 1,
		c,
		 (event.modifiers >> 8) & mode_mask,
		((event.modifiers >> 8) & attr_mask) | keypad | action,
	};
	
	write( fd, &buffer.len, sizeof buffer );
}

}
