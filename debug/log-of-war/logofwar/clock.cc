/*
	clock.cc
	--------
*/

#include "logofwar/clock.hh"

// POSIX
#include <sys/time.h>


namespace logofwar
{

#ifdef __MC68K__

static inline
asm
unsigned short divu_1000( unsigned x : __D0 )
{
	DIVU.W   #1000,D0
}

#else

static inline
unsigned short divu_1000( unsigned x )
{
	return x / 1000;
}

#endif

static
unsigned long long timeofday()
{
	timeval tv;
	gettimeofday( &tv, 0 );  // NULL
	
	return tv.tv_sec * 1000ull + divu_1000( tv.tv_usec );
}

static unsigned long long start = timeofday();

unsigned long long runclock()
{
	return timeofday() - start;
}

}
