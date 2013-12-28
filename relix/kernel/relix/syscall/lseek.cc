/*
	lseek.cc
	--------
*/

#include "relix/syscall/lseek.hh"

// vfs
#include "vfs/filehandle/functions/seek.hh"

// relix-kernel
#include "relix/api/errno.hh"
#include "relix/api/get_fd_handle.hh"


namespace relix
{
	
	off_t lseek( int fd, off_t offset, int whence )
	{
		try
		{
			return seek( get_fd_handle( fd ), offset, whence );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
	}
	
}

