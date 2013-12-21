/*
	getsockname.cc
	--------------
*/

#include "relix/syscall/getsockname.hh"

// vfs
#include "vfs/filehandle/primitives/getsockname.hh"

// relix-kernel
#include "relix/api/errno.hh"
#include "relix/api/get_fd_handle.hh"
#include "relix/socket/get_sockaddr_name.hh"


namespace relix
{
	
	int getsockname( int fd, struct sockaddr* name, socklen_t* namelen )
	{
		try
		{
			const sockaddr& addr = getsockname( get_fd_handle( fd ) );
			
			get_sockaddr_name( addr, name, namelen );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
}

