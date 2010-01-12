/*	====================
 *	GetStringResource.hh
 *	====================
 */

#ifndef GENIE_UTILITIES_GETSTRINGRESOURCE_HH
#define GENIE_UTILITIES_GETSTRINGRESOURCE_HH

// Standard C++
#include <string>

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif


namespace Genie
{
	
	std::string GetStringResource( ::ResID id );
	
}

#endif

