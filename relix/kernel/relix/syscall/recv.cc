/*
	recv.cc
	-------
*/

#include "relix/syscall/recv.hh"

// Standard C
#include <errno.h>

// relix
#include "relix/api/errno.hh"
#include "relix/syscall/read.hh"


namespace relix
{
	
	ssize_t recv( int fd, void* buf, size_t count, int flags )
	{
		if ( flags )
		{
			return set_errno( ENOSYS );
		}
		
		return read( fd, buf, count );
	}
	
}
