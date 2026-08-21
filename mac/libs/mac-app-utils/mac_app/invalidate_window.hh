/*
	invalidate_window.hh
	--------------------
*/

#ifndef MACAPP_INVALIDATEWINDOW_HH
#define MACAPP_INVALIDATEWINDOW_HH

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif


namespace mac {
namespace app {
	
	void invalidate_window( WindowRef window );
	
}
}

#endif
