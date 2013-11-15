/*
	dup3.cc
	-------
*/

#include "relix/syscall/dup3.hh"

// Standard C
#include <errno.h>

// relix-kernel
#include "relix/api/dup_fd.hh"
#include "relix/api/errno.hh"


#ifndef O_CLOEXEC
#define O_CLOEXEC  0
#endif

#ifndef DUP_DUP2
#define DUP_DUP2  (-1)
#endif


namespace relix
{
	
	int dup3( int old_fd, int new_fd, int flags )
	{
		const bool dup2_semantics = flags == DUP_DUP2;
		
		if ( dup2_semantics )
		{
			flags = 0;
		}
		
		if ( flags & ~O_CLOEXEC )
		{
			// Invalid flags
			return set_errno( EINVAL );
		}
		
		const bool close_on_exec = flags & O_CLOEXEC;
		
		try
		{
			dup_fd( old_fd, new_fd, close_on_exec );
			
			if ( old_fd == new_fd  &&  !dup2_semantics )
			{
				return set_errno( EINVAL );
			}
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return new_fd;
	}
	
}

