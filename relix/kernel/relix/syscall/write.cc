/*
	write.cc
	--------
*/

#include "relix/syscall/write.hh"

// vfs
#include "vfs/filehandle/primitives/write.hh"

// relix
#include "relix/api/errno.hh"
#include "relix/api/get_fd_handle.hh"


namespace relix
{
	
	ssize_t write( int fd, const void* buf, size_t count )
	{
		try
		{
			return write( get_fd_handle( fd ), (const char*) buf, count );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
	}
	
}

