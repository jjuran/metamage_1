/*	=======================
 *	CFStringGetStdString.hh
 *	=======================
 */

#ifndef GENIE_UTILITIES_CFSTRINGGETSTDSTRING_HH
#define GENIE_UTILITIES_CFSTRINGGETSTDSTRING_HH

// Mac OS
#ifndef __CFSTRING__
#include <CFString.h>
#endif

// plus
#include "plus/string.hh"


namespace Genie
{
	
	plus::string CFStringGetStdString( CFStringRef string );
	
}

#endif

