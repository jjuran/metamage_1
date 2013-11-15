/*
	ftruncate.cc
	------------
*/

#include "relix/syscall/ftruncate.hh"

// vfs
#include "vfs/filehandle/primitives/seteof.hh"

// relix
#include "relix/api/errno.hh"
#include "relix/api/get_fd_handle.hh"


namespace relix
{
	
	int ftruncate( int fd, off_t length )
	{
		try
		{
			seteof( get_fd_handle( fd ), length );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
}

