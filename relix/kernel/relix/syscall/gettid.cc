/*
	gettid.cc
	---------
*/

#include "relix/syscall/gettid.hh"

// relix-kernel
#include "relix/api/current_thread.hh"
#include "relix/task/thread.hh"


namespace relix
{
	
	int gettid()
	{
		return current_thread().id();
	}
	
}

