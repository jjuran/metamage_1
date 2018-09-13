/*
	clock.cc
	--------
*/

#include "v68k-time/clock.hh"

// POSIX
#include <sys/time.h>


#pragma exceptions off


namespace v68k {
namespace time {

uint64_t microsecond_clock()
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

const uint64_t initial_clock = microsecond_clock();

}  // namespace time
}  // namespace v68k
