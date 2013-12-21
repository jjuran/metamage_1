/*
	shutdown.cc
	-----------
*/

#include "relix/syscall/shutdown.hh"

// vfs
#include "vfs/filehandle/primitives/shutdown.hh"

// relix-kernel
#include "relix/api/errno.hh"
#include "relix/api/get_fd_handle.hh"


namespace relix
{
	
	int shutdown( int fd, int how )
	{
		try
		{
			shutdown( relix::get_fd_handle( fd ), how );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
}

