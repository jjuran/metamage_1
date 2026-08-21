/*
	windows.hh
	----------
*/

#include "mac_ui/windows.hh"


namespace mac {
namespace ui  {

#if ! __LP64__

Point get_window_position( WindowRef window )
{
#if ! OPAQUE_TOOLBOX_STRUCTS
	
	CGrafPtr port = (CGrafPtr) window;
	
	const Rect& bounds = port->portVersion < 0 ? port->portPixMap[0]->bounds
	                                           : window->portBits.bounds;
	
	Point position;
	
	position.v = -bounds.top;
	position.h = -bounds.left;
	
	return position;
	
#else
	
	Rect bounds;
	GetWindowBounds( window, kWindowGlobalPortRgn, &bounds );
	
	return (Point&) bounds;
	
#endif
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

OSStatus set_window_title( WindowRef window, const HFSUniStr255& name )
{
	OSStatus err = memFullErr;  // Why else would CFStringCreate... fail?
	
	if ( CFStringRef s = CFStringCreateWithCharacters( NULL, name.unicode,
	                                                         name.length ) )
	{
		err = SetWindowTitleWithCFString( window, s );
		
		CFRelease( s );
	}
	
	return err;
}

OSStatus set_window_title_UTF8( WindowRef window, const char* p, int n )
{
	OSStatus err = memFullErr;  // Why else would CFStringCreate... fail?
	
	CFStringRef s = CFStringCreateWithBytes( kCFAllocatorDefault,
	                                         (const Byte*) p,
	                                         n,
	                                         kCFStringEncodingUTF8,
	                                         false );
	
	if ( s )
	{
		err = SetWindowTitleWithCFString( window, s );
		
		CFRelease( s );
	}
	
	return err;
}

#else

int dummy;

#endif  // #if ! __LP64__

}
}
