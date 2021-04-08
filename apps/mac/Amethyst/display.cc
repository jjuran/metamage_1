/*
	display.cc
	----------
*/

#include "display.hh"

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef CGDIRECTDISPLAY_H_
#ifndef __CGDIRECT_DISPLAY_H__
#ifndef __CGDIRECTDISPLAY__
#include <CGDirectDisplay.h>
#endif
#endif
#endif

// Amethyst
#include "quickdraw.hh"


display_capture::display_capture( CGDirectDisplayID id ) : its_id( id )
{
	if ( CGError err = CGDisplayCapture( its_id ) )
	{
		throw CGDisplayCapture_Failed();
	}
	
	void* addr;
	
#if CONFIG_QUICKDRAW
	
	// UInt32 cast required for 10.4 PPC, at least
	
	addr =
	its_port = CreateNewPortForCGDisplayID( (UInt32) its_id );
	
#else
	
	addr =
	its_context = CGDisplayGetDrawingContext( its_id );
	
#endif
	
	if ( addr == NULL )
	{
		throw CGDisplayCapture_Failed();
	}
	
	CGDisplayHideCursor( its_id );
}

display_capture::~display_capture()
{
	CGDisplayShowCursor( its_id );
	
#if CONFIG_QUICKDRAW
	
	Rect bounds;
	PaintRect( GetPortBounds( its_port, &bounds ) );
	
	DisposePort( its_port );
	
#else
	
	CGRect bounds = CGDisplayBounds( its_id );
	
	/*
		We need both of these to clear the screen before releasing the capture.
		CGContextFlush() doesn't help.
	*/
	
	CGContextFillRect ( its_context, bounds );
	CGContextClearRect( its_context, bounds );
	
#endif
	
	CGDisplayRelease( its_id );
}
