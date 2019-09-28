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


display_capture::display_capture( CGDirectDisplayID id ) : its_id( id )
{
	if ( CGError err = CGDisplayCapture( its_id ) )
	{
		throw CGDisplayCapture_Failed();
	}
	
	its_context = CGDisplayGetDrawingContext( its_id );
	
	if ( its_context == NULL )
	{
		throw CGDisplayCapture_Failed();
	}
	
	CGDisplayHideCursor( its_id );
}

display_capture::~display_capture()
{
	CGDisplayShowCursor( its_id );
	
	CGRect bounds = CGDisplayBounds( its_id );
	
	/*
		We need both of these to clear the screen before releasing the capture.
		CGContextFlush() doesn't help.
	*/
	
	CGContextFillRect ( its_context, bounds );
	CGContextClearRect( its_context, bounds );
	
	CGDisplayRelease( its_id );
}

double display_capture::x_scale_factor( double width, double height ) const
{
	CGRect frame = CGDisplayBounds( its_id );
	
	double x_factor = frame.size.width  / width;
	double y_factor = frame.size.height / height;
	
	double factor = x_factor < y_factor ? x_factor : y_factor;
	
	if ( factor > 1 )
	{
		factor = (int) factor;
	}
	
	return factor;
}

CGRect display_capture::x_scaled_frame( double width, double height ) const
{
	CGRect frame = CGDisplayBounds( its_id );
	
	double factor = x_scale_factor( width, height );
	
	double dx = (frame.size.width  - factor * width ) / 2;
	double dy = (frame.size.height - factor * height) / 2;
	
	return CGRectInset( frame, dx, dy );
}
