/*
	time.cc
	-------
*/

#include "time.hh"

// POSIX
#include <sys/time.h>

// math
#include "math/integer.hh"


void time( timeval* now )
{
	gettimeofday( now, NULL );
}

timeval timeval_sub( const timeval& a, const timeval& b )
{
	time_t  sec  = a.tv_sec  - b.tv_sec;
	int32_t usec = a.tv_usec - b.tv_usec;
	
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
