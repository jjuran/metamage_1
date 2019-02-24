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

unsigned long long microseconds( const timeval& tv )
{
	using math::integer::long_multiply;
	
	return long_multiply( tv.tv_sec, 1000000 ) + tv.tv_usec;
}

unsigned long long time_microseconds()
{
	timeval now;
	gettimeofday( &now, NULL );
	
	return microseconds( now );
}
