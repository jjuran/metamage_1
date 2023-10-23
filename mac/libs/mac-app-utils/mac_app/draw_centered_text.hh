/*
	draw_centered_text.hh
	---------------------
*/

#ifndef MACAPP_DRAWCENTEREDTEXT_HH
#define MACAPP_DRAWCENTEREDTEXT_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif

// Mac OS
#ifndef __COREFOUNDATION_CFSTRING__
#ifndef __CFSTRING__
#include <CFString.h>
#endif
#endif


struct Rect;


namespace mac {
namespace app {
	
	void draw_centered_text( CFStringRef  text,
	                         Rect         bounds,
	                         const char*  fontName,
	                         UInt32       fontSize );
	
}
}

#endif
