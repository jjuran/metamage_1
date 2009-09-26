/*	=======================
 *	CFStringGetStdString.hh
 *	=======================
 */

#ifndef GENIE_UTILITIES_CFSTRINGGETSTDSTRING_HH
#define GENIE_UTILITIES_CFSTRINGGETSTDSTRING_HH

// Standard C++
#include <string>

// Mac OS
#include <CFString.h>


namespace Genie
{
	
	std::string CFStringGetStdString( CFStringRef string );
	
}

#endif

