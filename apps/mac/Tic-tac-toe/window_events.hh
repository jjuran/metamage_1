/*
	window_events.hh
	----------------
*/

#ifndef WINDOWEVENTS_HH
#define WINDOWEVENTS_HH

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

void install_window_event_handlers( WindowRef window );

#endif
