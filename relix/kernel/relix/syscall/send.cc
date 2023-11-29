/*
	send.cc
	-------
*/

#include "relix/syscall/send.hh"

// Standard C
#include <errno.h>

// relix
#include "relix/api/errno.hh"
#include "relix/syscall/write.hh"


namespace relix
{
	
	ssize_t send( int fd, const void* buf, size_t count, int flags )
	{
		if ( flags )
		{
			return set_errno( ENOSYS );
		}
		
		return write( fd, buf, count );
	}
	
}
