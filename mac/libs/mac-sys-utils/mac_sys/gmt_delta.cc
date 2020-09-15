/*
	gmt_delta.cc
	------------
*/

#include "mac_sys/gmt_delta.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __OSUTILS__
#include <OSUtils.h>
#endif


namespace mac {
namespace sys {

long gmtDelta_field()
{
	MachineLocation location;
	
	::ReadLocation( &location );
	
	long rawGMTDelta = location.u.gmtDelta;
	
	return location.u.gmtDelta;  // unmasked field value
}

}
}
