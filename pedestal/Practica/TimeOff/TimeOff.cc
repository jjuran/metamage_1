/*	==========
 *	TimeOff.cc
 *	==========
 */

#include "TimeOff.hh"

// Universal Interfaces
#include <OSUtils.h>


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

