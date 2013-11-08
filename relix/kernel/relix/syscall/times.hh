/*
	times.hh
	--------
*/

#ifndef RELIX_SYSCALL_TIMES_HH
#define RELIX_SYSCALL_TIMES_HH

// Standard C
#include <time.h>


struct tms;


namespace relix
{
	
	clock_t times( struct tms* tp );
	
}

#endif

