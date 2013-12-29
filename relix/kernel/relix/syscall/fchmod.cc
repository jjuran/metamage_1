/*
	fchmod.cc
	---------
*/

#include "relix/syscall/fchmod.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/primitives/chmod.hh"

// relix
#include "relix/api/errno.hh"
#include "relix/api/get_fd_handle.hh"


namespace relix
{
	
	int fchmod( int fd, mode_t mode )
	{
		try
		{
			chmod( *get_fd_handle( fd ).GetFile(), mode );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
}

