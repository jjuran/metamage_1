/*
	CenteredText.hh
	---------------
*/

#ifndef PEDESTAL_CENTEREDTEXT_HH
#define PEDESTAL_CENTEREDTEXT_HH

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


namespace Pedestal
{
	
	void DrawCenteredText( CFStringRef  text,
	                       Rect         bounds,
	                       const char*  fontName,
	                       UInt32       fontSize );
	
}

#endif
