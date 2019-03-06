/*
	splode.hh
	---------
*/

#ifndef SPLODE_HH
#define SPLODE_HH

// X11
#include <X11/Xlib.h>


extern bool swap_Command_and_Option;

void send_cursor_location( int x, int y );

void send_motion_event( XMotionEvent& event );
void send_button_event( XButtonEvent& event );
void send_key_event   ( XKeyEvent&    event );

#endif
