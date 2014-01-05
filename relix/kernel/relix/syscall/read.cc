/*
	read.cc
	-------
*/

#include "relix/syscall/read.hh"

// POSIX
#include <errno.h>
#include <fcntl.h>

// vfs
#include "vfs/filehandle.hh"
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
			vfs::filehandle& h = get_fd_handle( fd );
			
			if ( h.get_flags() & O_DIRECTORY )
			{
				return set_errno( EISDIR );
			}
			
			ssize_t get = read( h,
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

