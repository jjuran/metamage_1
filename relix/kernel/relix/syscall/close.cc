/*
	close.cc
	--------
*/

#include "relix/syscall/close.hh"

// relix-kernel
#include "relix/api/get_fds.hh"
#include "relix/api/errno.hh"
#include "relix/task/fd_map.hh"


namespace relix
{
	
	int close( int fd )
	{
		try
		{
			get_fds().close( fd );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
}

