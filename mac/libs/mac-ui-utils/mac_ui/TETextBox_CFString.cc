/*
	TETextBox_CFString.cc
	---------------------
*/

#include "mac_ui/TETextBox_CFString.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __TEXTEDIT__
#include <TextEdit.h>
#endif

// Standard C
#include <stdlib.h>


namespace mac {
namespace ui  {

#if ! __LP64__

void TETextBox_CFString( CFStringRef text, const Rect* rect, short just )
{
	CFIndex length = CFStringGetLength( text );
	
	CFRange range = CFRangeMake( 0, length );
	
	CFIndex usedBufLen = 0;
	
	(void) CFStringGetBytes( text,
	                         range,
	                         kCFStringEncodingMacRoman,
	                         '\0',
	                         false,
	                         NULL,
	                         0,
	                         &usedBufLen );
	
	Byte* buffer = (Byte*) alloca( usedBufLen + 1 );
	
	(void) CFStringGetBytes( text,
	                         range,
	                         kCFStringEncodingMacRoman,
	                         '\0',
	                         false,
	                         buffer,
	                         usedBufLen,
	                         &usedBufLen );
	
	TETextBox( buffer, usedBufLen, rect, just );
}

#else

int dummy;

#endif

}
}
