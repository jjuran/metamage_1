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
#ifndef __MC68K__
#ifndef __DRIVERSERVICES__
#include <DriverServices.h>
#endif
#endif
#ifndef __TIMER__
#include <Timer.h>
#endif

// math
#include "math/fractions.hh"
#include "math/integer.hh"

// mac-sys-utils
#if TARGET_CPU_68K
#include "mac_sys/trap_address.hh"
#endif


namespace mac {
namespace sys {

typedef UInt32 uint32_t;
typedef UInt64 uint64_t;

#if TARGET_CPU_68K

UInt32 Ticks : 0x016A;

short ROM85 : 0x028E;

enum
{
	_Microseconds  = 0xA193,
	_Unimplemented = 0xA89F,
};

static
bool has_Microseconds_trap()
{
	if ( ROM85 < 0 )
	{
		return false;  // 64K ROM never has _Microseconds
	}
	
	UniversalProcPtr microseconds  = get_trap_address( _Microseconds  );
	UniversalProcPtr unimplemented = get_trap_address( _Unimplemented );
	
	return microseconds != unimplemented;
}

static const bool has_Microseconds = has_Microseconds_trap();

#endif

void microseconds( unsigned long long* count )
{
#if TARGET_CPU_68K
	
	if ( ! has_Microseconds )
	{
		using math::fractions::div_65536_ULL;
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
			decimal point, we'll also multiply it by 2^16 (65536) -- so we
			get 1089589545 instead of 16625 (and twice the precision).
			At run time, after multiplying by the number of ticks, we then
			divide by 2^16 again, which at worst will rely on bit shifts.
			This reduces our error to about 65 microseconds per day.
			
			We define the term "chronofragment", which simply means a
			65,536,000,000th of a second.
			
				(Prior to switching the factor from 1,024 to 65,536,
				we required a different time unit, whose definition
				is retained below for historical reference.)
			
			Much like kibi- and mebi- are 1024x and 1048576x factors,
			counterparts to metric kilo- and mega- (1000x and 1000000x),
			we define the prefix mibi- to mean a factor of 1x/1024 (based
			on the metric milli- of 1x/1000), and the "mibimicrosecond",
			which is simply a 1,024,000,000th of a second.
		*/
		
		const uint64_t million = 1000 * 1000;
		const uint64_t trillion = million * million;
		
		const uint32_t tick_chronofragments = 65536 * trillion / 60147420;
		
		*count = long_multiply( Ticks, tick_chronofragments );
		
		div_65536_ULL( count );
		
		return;
	}
	
#else
	
	if ( &UpTime != 0 )
	{
		uint64_t now;
		(Nanoseconds&) now = AbsoluteToNanoseconds( UpTime() );
		
		*count = now / 1000;
		
		return;
	}
	
#endif
	
	Microseconds( (UnsignedWide*) count );
}

}
}
