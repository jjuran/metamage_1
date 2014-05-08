/*
	scheduler.hh
	------------
*/

#ifndef RELIX_TASK_SCHEDULER_HH
#define RELIX_TASK_SCHEDULER_HH

// POSIX
#include <sys/types.h>


namespace relix
{
	
	bool is_active();
	
	void mark_thread_active( pid_t tid );
	
	void mark_thread_inactive( pid_t tid );
	
}

#endif
