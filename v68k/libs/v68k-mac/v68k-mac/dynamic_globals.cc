/*
	dynamic_globals.cc
	------------------
*/

#include "v68k-mac/dynamic_globals.hh"

// POSIX
#include <sys/time.h>

// Standard C
#include <time.h>

// mac-types
#include "mac_types/epoch.hh"


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

uint32_t get_Time()
{
	return time( NULL ) + ::mac::types::epoch_delta();  // TODO:  Local time
}

}  // namespace mac
}  // namespace v68k
