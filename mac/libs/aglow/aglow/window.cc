/*
	window.cc
	---------
*/

#include "aglow/window.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// missing-macos
#ifdef MAC_OS_X_VERSION_10_7
#ifndef MISSING_QUICKDRAW_H
#include "missing/Quickdraw.h"
#endif
#endif

// mac-qd-utils
#include "mac_qd/main_display_bounds.hh"

// amicus
#include "amicus/resize.hh"


using amicus::get_proportional_coordinate;
using amicus::get_proportional_offset;

static inline
short rect_width( const Rect& r )
{
	return r.right - r.left;
}

static inline
short rect_height( const Rect& r )
{
	return r.bottom - r.top;
}

static
Rect get_desktop_bounds()
{
	Rect bounds = mac::qd::main_display_bounds();
	
	bounds.top    += 40;  // approximate menu bar height + title bar height
	bounds.bottom -=  2;  // approximate window frame thickness
	
	return bounds;
}

static Rect desktop_bounds = get_desktop_bounds();

const short desk_height = rect_height( desktop_bounds );
const short desk_width  = rect_width ( desktop_bounds );

static Point proportional_location =
{
	desk_height / 2u,
	desk_width  / 2u,
};

static
Point get_proportional_location( const Rect& bounds )
{
	short window_height = bounds.bottom - bounds.top;
	short window_width  = bounds.right - bounds.left;
	
	Point location =
	{
		get_proportional_coordinate( desk_height, window_height, bounds.top  ),
		get_proportional_coordinate( desk_width,  window_width,  bounds.left ),
	};
	
	return location;
}

static
Rect get_proportional_bounds( short width, short height, Point location )
{
	Rect bounds;
	
	bounds.top  = get_proportional_offset( desk_height, height, location.v );
	bounds.left = get_proportional_offset( desk_width,  width,  location.h );
	
	bounds.bottom = bounds.top + height;
	bounds.right = bounds.left + width;
	
	return bounds;
}

static
void SetWindowTitleToProcessName( WindowRef window )
{
	ProcessSerialNumber psn = { 0, kCurrentProcess };
	
	OSStatus err;
	CFStringRef name;
	
	err = CopyProcessName( &psn, &name );
	
	if ( err == noErr )
	{
		SetWindowTitleWithCFString( window, name );
		
		CFRelease( name );
	}
}

WindowRef create_window( unsigned width, unsigned height, WindowRef previous )
{
	const WindowAttributes attrs = kWindowCloseBoxAttribute
	                             | kWindowCollapseBoxAttribute
	                           #ifdef MAC_OS_X_VERSION_10_3
	                             | kWindowCompositingAttribute
	                           #endif
	                           #ifdef MAC_OS_X_VERSION_10_3
	                             | kWindowAsyncDragAttribute
	                           #endif
	                           #ifdef MAC_OS_X_VERSION_10_7
	                             | kWindowHighResolutionCapableAttribute
	                           #endif
	                             ;
	
	Rect bounds;
	
	if ( previous )
	{
		GetWindowBounds( previous, kWindowGlobalPortRgn, &bounds );
		
		OffsetRect( &bounds, -desktop_bounds.left, -desktop_bounds.top );
		
		proportional_location = get_proportional_location( bounds );
	}
	
	bounds = get_proportional_bounds( width,
	                                  height,
	                                  proportional_location );
	
	OffsetRect( &bounds, desktop_bounds.left, desktop_bounds.top );
	
	OSStatus err;
	WindowRef window;
	err = CreateNewWindow( kDocumentWindowClass, attrs, &bounds, &window );
	
	if ( err != noErr )
	{
		return NULL;
	}
	
	SetWindowTitleToProcessName( window );
	
	return window;
}
