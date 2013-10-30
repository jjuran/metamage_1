/*
	getppid.cc
	----------
*/

#include "relix/syscall/getppid.hh"

// relix-kernel
#include "relix/api/current_process.hh"
#include "relix/task/process.hh"


namespace relix
{
	
	int getppid()
	{
		return current_process().getppid();
	}
	
}

