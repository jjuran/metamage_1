/*
	scheduler.hh
	------------
*/

#ifndef GENIE_SCHEDULER_HH
#define GENIE_SCHEDULER_HH

// POSIX
#include <sys/types.h>


namespace Genie
{
	
	bool is_active();
	
	void mark_process_active( pid_t pid );
	
	void mark_process_inactive( pid_t pid );
	
}

#endif

