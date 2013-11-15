/*
	get_fd_handle.cc
	----------------
*/

#include "relix/api/get_fd_handle.hh"

// vfs
#include "vfs/file_descriptor.hh"

// relix-kernel
#include "relix/api/get_fd.hh"


namespace relix
{
	
	vfs::filehandle& get_fd_handle( int fd )
	{
		return *get_fd( fd ).handle;
	}
	
}

