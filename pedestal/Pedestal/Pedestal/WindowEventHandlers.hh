/*
	WindowEventHandlers.hh
	----------------------
*/

#ifndef PEDESTAL_WINDOWEVENTHANDLERS_HH
#define PEDESTAL_WINDOWEVENTHANDLERS_HH

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


namespace Pedestal
{
	
	OSStatus install_window_event_handlers( WindowRef window );
	
}

#endif
