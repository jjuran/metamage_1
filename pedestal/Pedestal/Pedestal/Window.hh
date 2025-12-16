/*	=========
 *	Window.hh
 *	=========
 */

#ifndef PEDESTAL_WINDOW_HH
#define PEDESTAL_WINDOW_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


namespace Pedestal
{
	
	void ResizeWindow( WindowRef window, Point newSize );
	
	void ResizingWindow( WindowRef window, Point start );
	
}

#endif
