/*
	assign_fd.cc
	------------
*/

#include "relix/api/assign_fd.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/file_descriptor.hh"

// relix-kernel
#include "relix/api/get_fds.hh"
#include "relix/task/fd_map.hh"


namespace relix
{
	
	namespace p7 = poseven;
	
	
	void assign_fd( int               fd,
	                vfs::filehandle&  handle,
	                bool              close_on_exec )
	{
		if ( fd < 0 )
		{
			p7::throw_errno( EBADF );
		}
		
		fd_map& files = get_fds();
		
		(files[ fd ] = vfs::filehandle_ptr( &handle )).set_to_close_on_exec( close_on_exec );
	}
	
}

