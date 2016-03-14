/*
	microseconds.cc
	---------------
*/

#include "mac_sys/microseconds.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __TIMER__
#include <Timer.h>
#endif


namespace mac {
namespace sys {
	
	void microseconds( unsigned long long* count )
	{
		Microseconds( (UnsignedWide*) count );
	}
	
}
}
