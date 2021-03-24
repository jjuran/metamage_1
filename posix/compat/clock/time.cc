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

int clock_getres( clockid_t clock_id, struct timespec* ts )
{
	ts->tv_sec  = 0;
	ts->tv_nsec = 1000;
	
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
		Microseconds( (UnsignedWide*) &now );
		
		ts->tv_sec  = now / million;
		ts->tv_nsec = now % million * 1000;
		
		return 0;
	}
	
	errno = EINVAL;
	return -1;
}

#endif  // #ifdef __RELIX__
