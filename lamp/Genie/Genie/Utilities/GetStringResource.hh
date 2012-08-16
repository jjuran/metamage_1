/*	====================
 *	GetStringResource.hh
 *	====================
 */

#ifndef GENIE_UTILITIES_GETSTRINGRESOURCE_HH
#define GENIE_UTILITIES_GETSTRINGRESOURCE_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// plus
#include "plus/string.hh"


namespace Genie
{
	
	plus::string GetStringResource( ::ResID id );
	
}

#endif

