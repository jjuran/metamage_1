/*
	time.cc
	-------
*/

#include "time.hh"

// Mac OS
#ifndef __TIMER__
#include <Timer.h>
#endif

// POSIX
#include <sys/time.h>

// math
#include "math/integer.hh"

// ams-common
#include "callouts.hh"


void time( timeval* now )
{
	UInt64 microseconds;
	
	Microseconds( (UnsignedWide*) &microseconds );
	
	timeval_from_microseconds( now, &microseconds );
}

timeval timeval_sub( const timeval& a, const timeval& b )
{
	time_t      sec  = a.tv_sec  - b.tv_sec;
	suseconds_t usec = a.tv_usec - b.tv_usec;
	
	if ( usec < 0 )
	{
		usec += 1000000;
		--sec;
	}
	
	timeval result = { sec, usec };
	
	return result;
}

bool timeval_less( const timeval& a, const timeval& b )
{
	return
		                          a.tv_sec  < b.tv_sec  ||
		a.tv_sec == b.tv_sec  &&  a.tv_usec < b.tv_usec;
}
