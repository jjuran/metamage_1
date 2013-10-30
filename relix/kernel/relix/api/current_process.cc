/*
	current_process.cc
	------------------
*/

#include "relix/api/current_process.hh"

// relix-api
#include "relix/api/current_thread.hh"

// relix-kernel
#include "relix/task/thread.hh"


namespace relix
{
	
	process& current_process()
	{
		return current_thread().get_process();
	}
	
}

