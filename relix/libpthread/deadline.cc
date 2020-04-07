/*
	deadline.cc
	-----------
*/

#include "deadline.hh"

// POSIX
#include <unistd.h>
#include <sys/time.h>

// Standard C
#include <time.h>


static bool operator<( const timespec& a, const timeval& b )
{
	if ( a.tv_sec > b.tv_sec )
	{
		return false;
	}
	
	if ( a.tv_sec == b.tv_sec  &&  a.tv_nsec > b.tv_usec * 1000 )
	{
		return false;
	}
	
	return true;
}

static inline bool operator>=( const timeval& a, const timespec& b )
{
	return b < a;
}

bool deadline_expired( const timespec& deadline )
{
	timeval now;
	
	gettimeofday( &now, NULL );
	
	return now >= deadline;
}
