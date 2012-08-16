/*
	SystemVersion.cc
	----------------
*/

#include "MacFeatures/SystemVersion.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __GESTALT__
#include <Gestalt.h>
#endif


namespace MacFeatures
{
	
	unsigned SystemVersion()
	{
		long result = 0;
		
		const OSErr err = ::Gestalt( gestaltSystemVersion, &result );
		
		(void) err;  // gestaltSystemVersion is always defined
		
		return result;
	}
	
}

