/*	====================
 *	GetStringResource.hh
 *	====================
 */

#ifndef GENIE_UTILITIES_GETSTRINGRESOURCE_HH
#define GENIE_UTILITIES_GETSTRINGRESOURCE_HH

// Standard C++
#include <string>

// Mac OS
#include <Resources.h>


namespace Genie
{
	
	std::string GetStringResource( ::ResID id );
	
}

#endif

