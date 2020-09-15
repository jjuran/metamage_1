/*
	clock.cc
	--------
*/

#include "logofwar/clock.hh"

// POSIX
#include <sys/time.h>


namespace logofwar
{

static
unsigned long long timeofday()
{
	timeval tv;
	gettimeofday( &tv, 0 );  // NULL
	
	return tv.tv_sec * 1000ull + tv.tv_usec / 1000;
}

static unsigned long long start = timeofday();

unsigned long long runclock()
{
	return timeofday() - start;
}

}
