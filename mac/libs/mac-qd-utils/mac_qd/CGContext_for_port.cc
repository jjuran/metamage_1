/*
	CGContext_for_port.cc
	---------------------
*/

#include "mac_qd/CGContext_for_port.hh"

// mac-qd-utils
#include "mac_qd/get_portRect.hh"


namespace mac {
namespace qd  {

#if ! __LP64__

CGContext_for_port::CGContext_for_port( CGrafPtr port )
{
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

#endif

CGContext_for_port::~CGContext_for_port()
{
	CGContextFlush  ( context );  // required in Mac OS X 10.4
	CGContextRelease( context );
}

}
}
