/*
	clock/time.cc
	-------------
*/

#include "clock/time.h"

// Mach
#ifdef __MACH__
#include <mach/mach_time.h>
#endif

// Mac OS
#ifdef __RELIX__
#ifndef __MC68K__
#ifndef __DRIVERSERVICES__
#include <DriverServices.h>
#endif
#endif
#ifndef __TIMER__
#include <Timer.h>
#endif
#endif

// POSIX
#include <sys/time.h>

// Standard C
#include <errno.h>


const unsigned million = 1000 * 1000;
const unsigned billion = 1000 * 1000 * 1000;

static inline
int get_realtime_clock( struct timespec* ts )
{
	timeval now;
	gettimeofday( &now, NULL );
	
	ts->tv_sec  = now.tv_sec;
	ts->tv_nsec = now.tv_usec * 1000;
	
	return 0;
}

#ifdef __MACH__

static
const mach_timebase_info_data_t& get_timebase_info()
{
	static mach_timebase_info_data_t timebase_info;
	
	if ( timebase_info.denom == 0 )
	{
		mach_timebase_info( &timebase_info );
		
		while ( ((timebase_info.numer | timebase_info.denom) & 1) == 0 )
		{
			timebase_info.numer >>= 1;
			timebase_info.denom >>= 1;
		}
	}
	
	return timebase_info;
}

#ifdef __MAC_10_12  // <time.h> includes <Availability.h>

int dummy;

#else

int clock_getres( clockid_t clock_id, struct timespec* ts )
{
	ts->tv_sec = 0;
	
	if ( clock_id == CLOCK_REALTIME )
	{
		ts->tv_nsec = 1000;
		return 0;
	}
	
	if ( clock_id == CLOCK_MONOTONIC )
	{
		const mach_timebase_info_data_t& timebase_info = get_timebase_info();
		
		ts->tv_nsec = timebase_info.numer / timebase_info.denom;
		return 0;
	}
	
	errno = EINVAL;
	return -1;
}

int clock_gettime( clockid_t clock_id, struct timespec* ts )
{
	if ( clock_id == CLOCK_REALTIME )
	{
		return get_realtime_clock( ts );
	}
	
	if ( clock_id == CLOCK_MONOTONIC )
	{
		const mach_timebase_info_data_t& timebase_info = get_timebase_info();
		
		const uint64_t max_ULL = 18446744073709551615ull;
		const uint64_t max_now = max_ULL / timebase_info.numer;
		
		uint64_t now = mach_absolute_time();
		
		int b = 0;
		
		for ( ;  now > max_now;  ++b )
		{
			now >>= 1;
		}
		
		now *= timebase_info.numer;
		now /= timebase_info.denom;
		
		now <<= b;
		
		ts->tv_sec  = now / billion;
		ts->tv_nsec = now % billion;
		
		return 0;
	}
	
	errno = EINVAL;
	return -1;
}

#endif  // #ifdef __MAC_10_12

#endif  // #ifdef __MACH__

#ifdef __RELIX__

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
	
	UniversalProcPtr microseconds  = GetOSTrapAddress     ( _Microseconds  );
	UniversalProcPtr unimplemented = GetToolboxTrapAddress( _Unimplemented );
	
	return microseconds != unimplemented;
}

const bool has_Microseconds = has_Microseconds_trap();

#else

const bool has_Microseconds = true;

#endif

const uint64_t ns_per_tick = billion * 100000ull / 6014742;  // 16625817

int clock_getres( clockid_t clock_id, struct timespec* ts )
{
	ts->tv_sec = 0;
	
#if TARGET_CPU_PPC
	
	if ( clock_id == CLOCK_MONOTONIC  &&  &UpTime != 0 )
	{
		uint64_t one = 1;
		uint64_t res;
		
		(Nanoseconds&) res = AbsoluteToNanoseconds( (const AbsoluteTime&) one );
		
		ts->tv_nsec = res;
		
		return 0;
	}
	
#endif
	
	ts->tv_nsec = has_Microseconds ? 1000 : ns_per_tick;
	
	return 0;
}

int clock_gettime( clockid_t clock_id, struct timespec* ts )
{
	if ( clock_id == CLOCK_REALTIME )
	{
		return get_realtime_clock( ts );
	}
	
	if ( clock_id == CLOCK_MONOTONIC )
	{
		uint64_t now;
		
	#if TARGET_CPU_PPC
		
		if ( &UpTime != 0 )
		{
			(Nanoseconds&) now = AbsoluteToNanoseconds( UpTime() );
			
			ts->tv_sec  = now / billion;
			ts->tv_nsec = now % billion;
			
			return 0;
		}
		
	#endif
		
	#if TARGET_CPU_68K
		
		if ( ! has_Microseconds )
		{
			now = Ticks * ns_per_tick;
			
			ts->tv_sec  = now / billion;
			ts->tv_nsec = now % billion;
			
			return 0;
		}
		
	#endif
		
		Microseconds( (UnsignedWide*) &now );
		
		ts->tv_sec  =           now / million;
		ts->tv_nsec = uint32_t( now % million ) * 1000;
		
		return 0;
	}
	
	errno = EINVAL;
	return -1;
}

#endif  // #ifdef __RELIX__
