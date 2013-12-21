/*
	connect.cc
	----------
*/

#include "relix/syscall/connect.hh"

// vfs
#include "vfs/filehandle/primitives/connect.hh"

// relix-kernel
#include "relix/api/errno.hh"
#include "relix/api/get_fd_handle.hh"


namespace relix
{
	
	int connect( int fd, const struct sockaddr* serv_addr, socklen_t addrlen )
	{
		try
		{
			connect( get_fd_handle( fd ), *serv_addr, addrlen );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
}

