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

static
int run_for_clock( clockid_t clock_id, const char* name )
{
	const int billion = 1000 * 1000 * 1000;
	
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
	
	const int n = 1024;
	
	for ( int i = 0;  i < n;  ++i )
	{
		clock_gettime( clock_id, &ts1 );
	}
	
	int64_t t0 = ts0.tv_sec * billion + ts0.tv_nsec;
	int64_t t1 = ts1.tv_sec * billion + ts1.tv_nsec;
	
	int64_t t = t1 - t0;
	
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
