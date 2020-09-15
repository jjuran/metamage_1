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


struct EventRecord;


namespace Pedestal
{
	
	void window_activated( WindowRef window, bool activating );
	
	void window_mouseDown( WindowRef window, const EventRecord& event );
	
	void window_update( WindowRef window );
	
	OSStatus install_window_event_handlers( WindowRef window );
	
}

#endif
