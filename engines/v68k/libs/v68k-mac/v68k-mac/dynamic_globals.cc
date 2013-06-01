/*
	dynamic_globals.cc
	------------------
*/

#include "v68k-mac/dynamic_globals.hh"

// POSIX
#include <sys/time.h>


#pragma exceptions off


namespace v68k {
namespace mac  {

static uint64_t microsecond_clock()
{
	timeval tv;
	
	int got = gettimeofday( &tv, NULL );
	
	if ( got < 0 )
	{
		return 0;
	}
	
	return + tv.tv_sec * 1000 * 1000
	       + tv.tv_usec;
}

static const uint64_t initial_clock = microsecond_clock();

uint32_t get_Ticks()
{
	const uint64_t delta = microsecond_clock() - initial_clock;
	
	const unsigned microseconds_per_tick = 1000 * 1000 / 60;
	
	return delta / microseconds_per_tick;
}

}  // namespace mac
}  // namespace v68k

