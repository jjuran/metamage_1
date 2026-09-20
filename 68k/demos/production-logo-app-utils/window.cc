/*
	window.cc
	---------
*/

#include "window.hh"

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif


#define LOWMEM( addr, type )  (*(type*) (addr))

#define CrsrPin     LOWMEM( 0x0834, Rect  )
#define PaintWhite  LOWMEM( 0x09DC, short )


WindowRef create_window()
{
	const Rect& bounds = CrsrPin;
	
	WindowRef window;
	
	PaintWhite = false;
	
	window = NewWindow( 0, &bounds, "\p", 1, plainDBox, (WindowRef) -1, 0, 0 );
	
	PaintWhite = true;
	
	SetPortWindowPort( window );
	
	const Rect& portRect = window->portRect;
	
	RectRgn( window->visRgn, &portRect );
	
	PaintRect( &portRect );
	ValidRect( &portRect );
	
	return window;
}
