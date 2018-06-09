/*
	dynamic_globals.cc
	------------------
*/

#include "v68k-mac/dynamic_globals.hh"

// POSIX
#include <sys/time.h>

// Standard C
#include <time.h>


#pragma exceptions off


namespace v68k {
namespace mac  {

static uint64_t microsecond_clock()
{
	timeval tv;
	
	int got = gettimeofday( &tv, 0 );  // NULL
	
	if ( got < 0 )
	{
		return 0;
	}
	
	return + tv.tv_sec * 1000000ull
	       + tv.tv_usec;
}

static const uint64_t initial_clock = microsecond_clock();

uint32_t get_Ticks()
{
	const uint64_t delta = microsecond_clock() - initial_clock;
	
	const unsigned microseconds_per_tick = 1000 * 1000 / 60;
	
	return delta / microseconds_per_tick;
}

static inline
unsigned long Mac_to_Unix_epoch_delta()
{
	/*
		Returns the number of seconds between Mac and Unix epochs (global time).
		
		Mac time - Unix time = delta
		Mac time - delta = Unix time
		Unix time + delta = Mac time
	*/
	
	enum { mac_year = 1904, unix_year = 1970 };
	
	const unsigned years = unix_year - mac_year;  // 66
	const unsigned quads = years / 4;  // 16
	const unsigned extra = years % 4;  // 2
	
	const unsigned days_per_quad = 4 * 365 + 1;  // One Feb 29 per four years
	const unsigned extra_days = extra * 365 + 1;  // First year is a leap year
	const unsigned delta_days = days_per_quad * quads + extra_days;
	
	const unsigned seconds_per_day = 60 * 60 * 24;
	const unsigned delta_seconds = seconds_per_day * delta_days;
	
	return delta_seconds;
}

uint32_t get_Time()
{
	return time( NULL ) + Mac_to_Unix_epoch_delta();  // TODO:  Local time
}

}  // namespace mac
}  // namespace v68k
