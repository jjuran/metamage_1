/*	==========
 *	TimeOff.cc
 *	==========
 */

#include "TimeOff/TimeOff.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __OSUTILS__
#include <OSUtils.h>
#endif


namespace TimeOff
{
	
	long GetGMTDeltaField()
	{
		MachineLocation location;
		
		::ReadLocation( &location );
		
		long rawGMTDelta = location.u.gmtDelta;
		
		return rawGMTDelta;
	}
	
}

