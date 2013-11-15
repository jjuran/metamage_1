/*
	get_fd.cc
	---------
*/

#include "relix/api/get_fd.hh"

// relix-kernel
#include "relix/api/get_fds.hh"
#include "relix/task/fd_map.hh"


namespace relix
{
	
	vfs::file_descriptor& get_fd( int fd )
	{
		return get_fds().at( fd );
	}
	
}

