/*	=======================
 *	CFStringGetStdString.hh
 *	=======================
 */

#ifndef GENIE_UTILITIES_CFSTRINGGETSTDSTRING_HH
#define GENIE_UTILITIES_CFSTRINGGETSTDSTRING_HH

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

// plus
#include "plus/string.hh"


namespace Genie
{
	
	plus::string CFStringGetStdString( CFStringRef       string,
	                                   CFStringEncoding  encoding = 0 );
	
}

#endif

