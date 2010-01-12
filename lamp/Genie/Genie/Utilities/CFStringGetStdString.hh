/*	=======================
 *	CFStringGetStdString.hh
 *	=======================
 */

#ifndef GENIE_UTILITIES_CFSTRINGGETSTDSTRING_HH
#define GENIE_UTILITIES_CFSTRINGGETSTDSTRING_HH

// Standard C++
#include <string>

// Mac OS
#ifndef __CFSTRING__
#include <CFString.h>
#endif


namespace Genie
{
	
	std::string CFStringGetStdString( CFStringRef string );
	
}

#endif

