/*
	splode.cc
	---------
*/

#include "splode.hh"

// X11
#include <X11/keysym.h>
#include <X11/Xlib.h>

// POSIX
#include <unistd.h>

// iota
#include "iota/endian.hh"

// splode
#include "splode/splode.hh"


#define WRITE( buffer )  write( STDOUT_FILENO, &buffer, sizeof buffer )


bool swap_Command_and_Option;

static uint8_t mode_state[ 16 ];

static const uint8_t mode_shift[ 16 ] =
{
	0,
	1,  // Shift L
	1,  // Shift R
	4,  // Control L
	
	4,  // Control R
	0,  // Caps Lock
	0,  // Shift Lock
	5,  // Meta L
	
	5,  // Meta R
	2,  // Alt L
	2,  // Alt R
	6,  // Super L
	
	6,  // Super R
	7,  // Hyper L
	7,  // Hyper R
	0,
};


void send_cursor_location( int x, int y )
{
	splode::pointer_location_buffer buffer =
	{
		sizeof buffer - 1,
		0,
		(int16_t) iota::big_u16( x ),
		(int16_t) iota::big_u16( y ),
	};
	
	WRITE( buffer );
}

void send_motion_event( XMotionEvent& event )
{
	send_cursor_location( event.x, event.y );
}

void send_button_event( XButtonEvent& event )
{
	uint8_t modes = 0;
	uint8_t attrs = 1 + (event.type == ButtonRelease);
	
	splode::pointer_event_buffer buffer =
	{
		sizeof buffer - 1,
		modes,
		attrs,
	};
	
	WRITE( buffer );
}

void send_key_event( XKeyEvent& event )
{
	const bool pressed = event.type == KeyPress;
	
	KeySym keysym = XLookupKeysym( &event, 0 );
	
	unsigned char c = keysym;
	
	switch ( keysym )
	{
		case XK_Delete:
			c = 0x7F;
			break;
		
		case XK_Up:
		case XK_Right:
			c ^= 0x01;
			// fall through
		
		case XK_Left:
		case XK_Down:
			c -= 0x51 - 0x1C;
			break;
		
		case XK_Shift_L:
		case XK_Shift_R:
		case XK_Control_L:
		case XK_Control_R:
		case XK_Meta_L:
		case XK_Meta_R:
		case XK_Alt_L:
		case XK_Alt_R:
		case XK_Super_L:
		case XK_Super_R:
		case XK_Hyper_L:
		case XK_Hyper_R:
			mode_state[ keysym & 0xF ] = pressed << mode_shift[ keysym & 0xF ];
			c = '\0';
			break;
		
		default:
			break;
	}
	
	uint8_t modes = mode_state[ 1 ]
	              | mode_state[ 2 ]
	              | mode_state[ 3 ]
	              | mode_state[ 4 ]
	              | mode_state[ 5 ]
	              | mode_state[ 6 ]
	              | mode_state[ 7 ]
	              | mode_state[ 8 ]
	              | mode_state[ 9 ]
	              | mode_state[ 10 ]
	              | mode_state[ 11 ]
	              | mode_state[ 12 ]
	              | mode_state[ 13 ]
	              | mode_state[ 14 ];
	
	using namespace splode::modes;
	
	if ( swap_Command_and_Option )
	{
		modes |= !! (modes & Super) << Option_bit;   // Super -> Option
		modes |= !! (modes & Alt  ) << Command_bit;  // Alt -> Command
	}
	else
	{
		modes |= !! (modes & Alt  ) << Option_bit;   // Alt -> Option
		modes |= !! (modes & Super) << Command_bit;  // Super -> Command
	}
	
	uint8_t attrs = 2 - pressed;
	
	splode::ascii_event_buffer buffer =
	{
		sizeof buffer - 1,
		c,
		modes,
		attrs,
	};
	
	WRITE( buffer );
}
