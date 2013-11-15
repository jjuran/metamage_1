/*
	pread.cc
	--------
*/

#include "relix/syscall/pread.hh"

// Standard C
#include <errno.h>

// vfs
#include "vfs/filehandle/primitives/pread.hh"

// relix
#include "relix/api/errno.hh"
#include "relix/api/get_fd_handle.hh"


namespace relix
{
	
	ssize_t pread( int fd, void* buf, size_t count, off_t offset )
	{
		if ( offset < 0 )
		{
			return set_errno( EINVAL );
		}
		
		try
		{
			ssize_t get = pread( get_fd_handle( fd ),
			                     (char*) buf,
			                     count,
			                     offset );
			
			return get;
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
	}
	
}

