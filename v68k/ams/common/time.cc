/*
	time.cc
	-------
*/

#include "time.hh"

// POSIX
#include <sys/time.h>


unsigned long long microseconds( const timeval& tv )
{
	return tv.tv_sec * 1000000ull + tv.tv_usec;
}

unsigned long long time_microseconds()
{
	timeval now;
	gettimeofday( &now, NULL );
	
	return microseconds( now );
}
