/*
	get_fds.cc
	----------
*/

#include "relix/api/get_fds.hh"

// relix-kernel
#include "relix/api/current_process.hh"
#include "relix/task/process.hh"
#include "relix/task/process_resources.hh"


namespace relix
{
	
	fd_map& get_fds()
	{
		return current_process().get_process_resources().get_fd_map();
	}
	
}

