/*
	get_fd.hh
	---------
*/

#ifndef RELIX_API_GETFD_HH
#define RELIX_API_GETFD_HH

// vfs
#include "vfs/file_descriptor_fwd.hh"


namespace relix
{
	
	vfs::file_descriptor& get_fd( int fd );
	
}

#endif

