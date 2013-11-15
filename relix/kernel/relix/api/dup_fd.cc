/*
	dup_fd.cc
	---------
*/

#include "relix/api/dup_fd.hh"

// relix-kernel
#include "relix/api/assign_fd.hh"
#include "relix/api/get_fd_handle.hh"


namespace relix
{
	
	int dup_fd( int   old_fd,
	            int   new_fd,
	            bool  close_on_exec )
	{
		// Throws EBADF
		vfs::filehandle& handle = get_fd_handle( old_fd );
		
		if ( old_fd != new_fd )
		{
			assign_fd( new_fd, handle, close_on_exec );
		}
		
		return new_fd;
	}
	
}

