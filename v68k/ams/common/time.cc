/*
	time.cc
	-------
*/

#include "time.hh"

// POSIX
#include <sys/time.h>

// math
#include "math/integer.hh"


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
