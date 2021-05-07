/*
	clock_getrestime.cc
	-------------------
*/

// POSIX
#include <sys/time.h>

// Standard C
#include <stdint.h>
#include <stdio.h>
#include <time.h>

// compat
#include "clock/time.h"


#define RUN(clock)  run_for_clock(clock, #clock)

const uint64_t billion = 1000 * 1000 * 1000;

static inline
int64_t nanoseconds( const timespec& ts )
{
	return ts.tv_sec * billion + ts.tv_nsec;
}

static
int run_for_clock( clockid_t clock_id, const char* name )
{
	printf( "%s:\n", name );
	
	timespec res;
	timespec ts0, ts1;
	
	if ( clock_getres( clock_id, &res ) < 0 )
	{
		perror( "clock_getres" );
		return 1;
	}
	
	printf( "    res:  %ldns\n", (long) res.tv_nsec );
	
	if ( clock_gettime( clock_id, &ts0 ) < 0 )
	{
		perror( "clock_gettime" );
		return 2;
	}
	
	printf( "    time: %ld.%.9ld\n",
	        (long) ts0.tv_sec,
	        (long) ts0.tv_nsec );
	
	clock_gettime( clock_id, &ts0 );
	
	unsigned n = 16;
	
retry:
	
	for ( int i = 0;  i < n;  ++i )
	{
		clock_gettime( clock_id, &ts1 );
	}
	
	int64_t t = nanoseconds( ts1 ) - nanoseconds( ts0 );
	
	if ( t < 1000000 )
	{
		n *= 16;
		goto retry;
	}
	
	t /= n;
	
	printf( "    cost: %ldns\n\n", (long) t );
	
	return 0;
}

int main()
{
	int status = 0;
	
	status |= RUN( CLOCK_REALTIME  );
	status |= RUN( CLOCK_MONOTONIC );
	
	return status;
}
