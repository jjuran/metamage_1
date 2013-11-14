/*
	assign_fd.hh
	------------
*/

#ifndef RELIX_API_ASSIGNFD_HH
#define RELIX_API_ASSIGNFD_HH

// vfs
#include "vfs/filehandle_fwd.hh"


namespace relix
{
	
	void assign_fd( int               fd,
	                vfs::filehandle&  handle,
	                bool              close_on_exec = false );
	
}

#endif

