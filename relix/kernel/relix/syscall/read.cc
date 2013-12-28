/*
	read.cc
	-------
*/

#include "relix/syscall/read.hh"

// vfs
#include "vfs/filehandle/primitives/read.hh"

// relix
#include "relix/api/errno.hh"
#include "relix/api/get_fd_handle.hh"


namespace relix
{
	
	ssize_t read( int fd, void* buf, size_t count )
	{
		try
		{
			ssize_t get = read( get_fd_handle( fd ),
			                    (char*) buf,
			                    count );
			
			return get;
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
	}
	
}

