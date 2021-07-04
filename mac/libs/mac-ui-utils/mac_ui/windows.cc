/*
	windows.hh
	----------
*/

#include "mac_ui/windows.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif


namespace mac {
namespace ui  {

#if ! __LP64__

Point get_window_position( WindowRef window )
{
#if ! TARGET_API_MAC_CARBON
	
	WindowPeek w = (WindowPeek) window;
	
	Point position;
	
	position.v  = -window->portBits.bounds.top;
	position.h = -window->portBits.bounds.left;
	
	return position;
	
#endif
	
	Rect bounds;
	GetWindowBounds( window, kWindowGlobalPortRgn, &bounds );
	
	return (Point&) bounds;
}

Point get_window_size( WindowRef window )
{
#if ! TARGET_API_MAC_CARBON
	
	Rect& bounds = window->portRect;
	
#else
	
	Rect bounds;
	GetWindowBounds( window, kWindowGlobalPortRgn, &bounds );
	
#endif
	
	Point size = { bounds.bottom - bounds.top, bounds.right - bounds.left };
	
	return size;
}

#else

int dummy;

#endif  // #if ! __LP64__

}
}
