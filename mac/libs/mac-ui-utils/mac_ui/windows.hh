/*
	windows.hh
	----------
*/

#ifndef MACUI_WINDOWS_HH
#define MACUI_WINDOWS_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


namespace mac {
namespace ui  {
	
	Point get_window_position( WindowRef window );
	
	Point get_window_size( WindowRef window );
	
	bool invalidate_if_compositing( WindowRef window );
	
	void invalidate_window( WindowRef window );
	
	OSStatus set_window_title_UTF8( WindowRef window, const char* p, int n );
	
}
}

#endif
