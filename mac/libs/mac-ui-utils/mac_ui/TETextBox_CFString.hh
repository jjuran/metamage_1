/*
	TETextBox_CFString.hh
	---------------------
*/

#ifndef MACUI_TETEXTBOXCFSTRING_HH
#define MACUI_TETEXTBOXCFSTRING_HH

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
namespace ui  {
	
	void TETextBox_CFString( CFStringRef text, const Rect* rect, short just );
	
}
}

#endif
