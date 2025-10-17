/*
	CGContextForPort.cc
	-------------------
*/

#include "CGContextForPort.hh"

// mac-qd-utils
#include "mac_qd/get_portRect.hh"


using mac::qd::get_portRect;


CGContextForPort::CGContextForPort()
{
	CGrafPtr port = GetQDGlobalsThePort();
	
	CreateCGContextForPort( port, &context );
	
	const Rect& portRect = get_portRect( port );
	
	bounds_rect = CGRectMake( 0, 0, portRect.right, portRect.bottom );
	
	/*
		Flip the coordinate system so that the Y axis increases
		downward as in QuickDraw (and compositing-mode windows).
	*/
	
	CGContextTranslateCTM( context, 0, bounds_rect.size.height );
	CGContextScaleCTM    ( context, 1, -1 );
}

CGContextForPort::~CGContextForPort()
{
	CGContextFlush  ( context );  // required in Mac OS X 10.4
	CGContextRelease( context );
}
