/*
	fsync.cc
	--------
*/

#include "relix/syscall/fsync.hh"

// vfs
#include "vfs/filehandle/primitives/fsync.hh"

// relix-kernel
#include "relix/api/errno.hh"
#include "relix/api/get_fd_handle.hh"


namespace relix
{
	
	int fsync( int fd )
	{
		try
		{
			fsync( relix::get_fd_handle( fd ) );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
}

