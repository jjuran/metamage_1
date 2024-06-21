/*
	CGContextForPort.cc
	-------------------
*/

#include "CGContextForPort.hh"

// mac-qd-utils
#include "mac_qd/get_portRect.hh"


using mac::qd::get_portRect;


Rect CGContextForPort::create()
{
	CGrafPtr port = GetQDGlobalsThePort();
	
	CreateCGContextForPort( port, &context );
	
	const Rect& portRect = get_portRect( port );
	
	return portRect;
}

void CGContextForPort::erase( const Rect& portRect, float gray )
{
	CGRect whole = {};
	
	whole.size.width  = portRect.right;
	whole.size.height = portRect.bottom;
	
	CGContextSetGrayFillColor( context, gray, 1 );
	
	CGContextFillRect( context, whole );
}

void CGContextForPort::transform( int height )
{
	/*
		Flip the coordinate system so that the Y axis increases
		downward as in QuickDraw (and compositing-mode windows).
	*/
	
	CGContextTranslateCTM( context, 0, height );
	CGContextScaleCTM    ( context, 1, -1 );
}

CGContextForPort::CGContextForPort()
{
	const Rect& portRect = create();
	
	transform( portRect.bottom );
}

CGContextForPort::CGContextForPort( float erase_gray )
{
	const Rect& portRect = create();
	
	erase( portRect, erase_gray );
	
	transform( portRect.bottom );
}

CGContextForPort::~CGContextForPort()
{
	CGContextFlush  ( context );  // required in Mac OS X 10.4
	CGContextRelease( context );
}
