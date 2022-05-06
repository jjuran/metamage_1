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

// math
#include "math/integer.hh"

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
		using math::integer::long_multiply;
		
		/*
			The actual frame rate of an original Macintosh is 60.[147420]
			(where the bracketed portion is a repeating decimal.
			To convert ticks to microseconds, we need to divide by the
			number of ticks per second (the frame rate) and multiply by
			one million (the number of microseconds per second).
			
			We'd like to avoid actually executing a divide instruction,
			though.  One way is to divide one million by 60.14742 and use
			the quotient as a multiplier.  Dividing yields 16625.817034,
			so we could use 16625 for integer math, at the cost of error:
			We lose 817 microseconds every 16.6 seconds or so, about three
			milliseconds per minute, 177ms per hour, or 4.25s per day.
			
			While that's too much error to accept, the idea is sound -- we
			just need more precision.  Besides augmenting the dividend of
			one million by another million factor to remove the divisor's
			decimal point, we'll also multiply it by 2^10 (1024) -- so we
			get 17024836 instead of 16625 (and more bits of precision).
			At run time, after multiplying by the number of ticks, we then
			divide by 2^10 again, which at worst will rely on bit shifts.
			This reduces our error to about four milliseconds per day.
			
			Much like kibi- and mebi- are 1024x and 1048576x factors,
			counterparts to metric kilo- and mega- (1000x and 1000000x),
			we define the prefix mibi- to mean a factor of 1x/1024 (based
			on the metric milli- of 1x/1000), and the "mibimicrosecond",
			which is simply a 1,024,000,000th of a second.
		*/
		
		const uint64_t million = 1000 * 1000;
		const uint64_t trillion = million * million;
		
		const uint32_t tick_mibimicroseconds = 1024 * trillion / 60147420;
		
		*count = long_multiply( LMGetTicks(), tick_mibimicroseconds ) >> 10;
		
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
