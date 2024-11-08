/*
	splode.cc
	---------
*/

#include "amicus/splode.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// splode
#include "splode/splode.hh"

// write-a-splode
#include "splode/write-a-splode.hh"

// amicus
#include "amicus/keycodes.hh"


namespace amicus
{

void send_mouse_event( int fd, const EventRecord& event )
{
	using namespace splode::modes;
	
	const uint8_t mode_mask = Command | Shift | Option | Control;
	
	const uint8_t general_modifiers = (event.modifiers >> 8) & mode_mask;
	const uint8_t button_attributes = event.what;  // Yes, the codes match.
	
	splode::send_mouse_event( fd, general_modifiers, button_attributes );
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
	
	uint8_t modes =  (event.modifiers >> 8) & mode_mask;
	uint8_t attrs = ((event.modifiers >> 8) & attr_mask) | keypad | action;
	
	splode::send_key_event( fd, c, modes, attrs );
}

}
