/*
	task.hh
	-------
*/

#ifndef RELIX_TASK_TASK_HH
#define RELIX_TASK_TASK_HH

// relix
#include "relix/task/queue.hh"


namespace relix
{
	
	struct task : queue_element
	{
		bool pending;
	};
	
}

#endif
