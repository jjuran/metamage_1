/*
	getpid.cc
	---------
*/

#include "relix/syscall/getpid.hh"

// relix-kernel
#include "relix/api/current_process.hh"
#include "relix/task/process.hh"


namespace relix
{
	
	int getpid()
	{
		return current_process().id();
	}
	
}

