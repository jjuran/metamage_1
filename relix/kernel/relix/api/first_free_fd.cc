/*
	first_free_fd.cc
	----------------
*/

#include "relix/api/first_free_fd.hh"

// relix-kernel
#include "relix/api/get_fds.hh"
#include "relix/task/fd_map.hh"


namespace relix
{
	
	int first_free_fd( int min_fd )
	{
		return get_fds().first_unused( min_fd );
	}
	
}

