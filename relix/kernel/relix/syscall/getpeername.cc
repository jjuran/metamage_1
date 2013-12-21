/*
	getpeername.cc
	--------------
*/

#include "relix/syscall/getpeername.hh"

// vfs
#include "vfs/filehandle/primitives/getpeername.hh"

// relix-kernel
#include "relix/api/errno.hh"
#include "relix/api/get_fd_handle.hh"
#include "relix/socket/get_sockaddr_name.hh"


namespace relix
{
	
	int getpeername( int fd, struct sockaddr* name, socklen_t* namelen )
	{
		try
		{
			const sockaddr& addr = getpeername( get_fd_handle( fd ) );
			
			get_sockaddr_name( addr, name, namelen );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
}

