/*
	listen.cc
	---------
*/

#include "relix/syscall/listen.hh"

// vfs
#include "vfs/filehandle/primitives/listen.hh"

// relix-kernel
#include "relix/api/errno.hh"
#include "relix/api/get_fd_handle.hh"


namespace relix
{
	
	int listen( int fd, int backlog )
	{
		try
		{
			listen( relix::get_fd_handle( fd ), backlog );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
}

