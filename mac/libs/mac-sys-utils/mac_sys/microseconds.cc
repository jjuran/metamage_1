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
#ifndef __DRIVERSERVICES__
#include <DriverServices.h>
#endif
#ifndef __LOWMEM__
#include <LowMem.h>
#endif
#ifndef __TIMER__
#include <Timer.h>
#endif
#ifdef __MACOS__
#ifndef __TRAPS__
#include <Traps.h>
#endif
#endif

// mac-sys-utils
#include "mac_sys/trap_available.hh"


namespace mac {
namespace sys {
	
	void microseconds( unsigned long long* count )
	{
	#if TARGET_CPU_68K
		
		static const bool available = trap_available( _Microseconds );
		
		if ( ! available )
		{
			*count = LMGetTicks() * 100000000ull / 6015;  // * (10^6 / 60.15)
			
			return;
		}
		
	#endif
		
		if ( ! TARGET_CPU_68K  &&  &UpTime != 0 )
		{
			uint64_t now;
			(Nanoseconds&) now = AbsoluteToNanoseconds( UpTime() );
			
			*count = now / 1000;
			
			return;
		}
		
		Microseconds( (UnsignedWide*) count );
	}
	
}
}
