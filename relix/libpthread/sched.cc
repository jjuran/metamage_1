/*
	sched.cc
	--------
*/

// POSIX
#include <sched.h>
#include <unistd.h>


int sched_yield()
{
	sleep( 0 );
	
	return 0;
}

