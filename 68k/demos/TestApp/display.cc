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
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


static
Rect Rect_from_CGRect( const CGRect& r )
{
	Rect result;
	
	result.top    = (short) CGRectGetMinY( r );
	result.left   = (short) CGRectGetMinX( r );
	result.bottom = (short) CGRectGetMaxY( r );
	result.right  = (short) CGRectGetMaxX( r );
	
	return result;
}

const Rect& main_display_bounds()
{
#if ! TARGET_API_MAC_CARBON
	
	return qd.screenBits.bounds;
	
#endif
	
#if ! TARGET_API_MAC_OSX
	
	return GetMainDevice()[0]->gdRect;
	
#else
	
	static Rect rect = Rect_from_CGRect( CGDisplayBounds( CGMainDisplayID() ) );
	
	return rect;
	
#endif
}
