/*
	bind.cc
	-------
*/

#include "relix/syscall/bind.hh"

// vfs
#include "vfs/filehandle/primitives/bind.hh"

// relix-kernel
#include "relix/api/errno.hh"
#include "relix/api/get_fd_handle.hh"


namespace relix
{
	
	int bind( int fd, const struct sockaddr* name, socklen_t namelen )
	{
		try
		{
			bind( get_fd_handle( fd ), *name, namelen );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
}

