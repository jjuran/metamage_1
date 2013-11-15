/*
	get_fd_handle.hh
	----------------
*/

#ifndef RELIX_API_GETFDHANDLE_HH
#define RELIX_API_GETFDHANDLE_HH

// vfs
#include "vfs/filehandle_fwd.hh"


namespace relix
{
	
	vfs::filehandle& get_fd_handle( int fd );
	
}

#endif

