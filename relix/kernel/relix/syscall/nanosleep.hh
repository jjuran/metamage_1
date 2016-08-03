/*
	nanosleep.hh
	------------
*/

#ifndef RELIX_SYSCALL_NANOSLEEP_HH
#define RELIX_SYSCALL_NANOSLEEP_HH

// POSIX
//#include <sys/time.h>
struct timespec;


namespace relix
{
	
	int nanosleep( const timespec* requested, timespec* remaining );
	
}

#endif
