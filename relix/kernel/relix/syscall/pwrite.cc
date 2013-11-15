/*
	pwrite.cc
	---------
*/

#include "relix/syscall/pwrite.hh"

// Standard C
#include <errno.h>

// vfs
#include "vfs/filehandle/primitives/pwrite.hh"

// relix
#include "relix/api/errno.hh"
#include "relix/api/get_fd_handle.hh"


namespace relix
{
	
	ssize_t pwrite( int fd, const void* buf, size_t count, off_t offset )
	{
		if ( offset < 0 )
		{
			return set_errno( EINVAL );
		}
		
		try
		{
			vfs::filehandle& file = get_fd_handle( fd );
			
			const char* buffer = reinterpret_cast< const char* >( buf );
			
			const ssize_t put = pwrite( file, buffer, count, offset );
			
			return put;
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
	}
	
}

