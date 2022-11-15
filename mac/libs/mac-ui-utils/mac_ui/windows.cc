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

// mac-qd-utils
#include "mac_qd/get_portRect.hh"
#include "mac_qd/globals/thePort.hh"
#include "mac_qd/is_port_visrgn_empty.hh"


namespace mac {
namespace ui  {

#if ! __LP64__

Point get_window_position( WindowRef window )
{
#if ! OPAQUE_TOOLBOX_STRUCTS
	
	Point position;
	
	position.v = -window->portBits.bounds.top;
	position.h = -window->portBits.bounds.left;
	
	return position;
	
#endif
	
	Rect bounds;
	GetWindowBounds( window, kWindowGlobalPortRgn, &bounds );
	
	return (Point&) bounds;
}

Point get_window_size( WindowRef window )
{
#if ! OPAQUE_TOOLBOX_STRUCTS
	
	Rect& bounds = window->portRect;
	
#else
	
	Rect bounds;
	GetWindowBounds( window, kWindowGlobalPortRgn, &bounds );
	
#endif
	
	Point size = { bounds.bottom - bounds.top, bounds.right - bounds.left };
	
	return size;
}

#ifdef MAC_OS_X_VERSION_10_2

static
ControlRef get_content_view_if_compositing( WindowRef window )
{
#ifdef MAC_OS_X_VERSION_10_2
	
	OSStatus err;
	
#ifndef MAC_OS_X_VERSION_10_4
	
	// Mac OS X 10.2 - 10.3:  Check if the window is in compositing mode.
	
	WindowAttributes attrs = kWindowNoAttributes;
	err = GetWindowAttributes( window, &attrs );
	
	if ( err != noErr  ||  ! (attrs & kWindowCompositingAttribute) )
	{
		return NULL;
	}
	
	// The window is in compositing mode, so return the content view.
	
#endif
	
	// Mac OS X 10.2 - 10.14:  Get the window's content view.
	
	ControlRef content;
	err = GetRootControl( window, &content );
	
	if ( err != noErr )
	{
		return NULL;
	}
	
#ifdef MAC_OS_X_VERSION_10_4
	
	// Mac OS X 10.4 - 10.14:  Check if the view is in compositing mode.
	
	if ( ! HIViewIsCompositingEnabled( content ) )
	{
		return NULL;
	}
	
	// The content view is in compositing mode, so return it.
	
#endif
	
	return content;
	
#endif  // #ifdef MAC_OS_X_VERSION_10_2
	
	// Compositing mode doesn't exist.
	
	return NULL;
}

#endif  // #ifdef MAC_OS_X_VERSION_10_2

bool invalidate_if_compositing( WindowRef window )
{
#ifdef MAC_OS_X_VERSION_10_2
	
	OSStatus err;
	
	if ( ControlRef content = get_content_view_if_compositing( window ) )
	{
		err = HIViewSetNeedsDisplay( content, true );
		
		return err == noErr;
	}
	
#endif
	
	return false;
}

void invalidate_window( WindowRef window )
{
#ifdef MAC_OS_X_VERSION_10_2
	
	if ( invalidate_if_compositing( window ) )
	{
		return;
	}
	
#endif
	
	CGrafPtr port = GetWindowPort( window );
	
	if ( ! mac::qd::is_port_visrgn_empty( port ) )
	{
		const Rect& portRect = mac::qd::get_portRect( port );
		
	#if ! OPAQUE_TOOLBOX_STRUCTS
		
		GrafPtr thePort = mac::qd::thePort();
		
		if ( thePort != window )
		{
			SetPort( window );
		}
		
		InvalRect( &portRect );
		
		if ( thePort != window )
		{
			SetPort( thePort );
		}
		
		return;
		
	#endif
		
		InvalWindowRect( window, &portRect );
	}
}

#else

int dummy;

#endif  // #if ! __LP64__

}
}
