/*
	unshare.cc
	----------
*/

#include "relix/syscall/unshare.hh"

// Standard C
#include <errno.h>

// relix-include
#include "relix/sched.h"

// relix-kernel
#include "relix/api/current_process.hh"
#include "relix/api/errno.hh"
#include "relix/task/process.hh"
#include "relix/task/process_resources.hh"


namespace relix
{
	
	const int supported_clone_flags = CLONE_FS
	                                | CLONE_FILES
	                                | CLONE_NEWNS;
	
	int unshare( int flags )
	{
		if ( flags & ~supported_clone_flags )
		{
			// unsupported flag
			return set_errno( EINVAL );
		}
		
		const bool unshare_fs    = flags & CLONE_FS;
		const bool unshare_files = flags & CLONE_FILES;
		const bool unshare_newns = flags & CLONE_NEWNS;
		
		if ( unshare_newns )
		{
			return set_errno( EPERM );
		}
		
		try
		{
			process_resources& caller = current_process().get_process_resources();
			
			if ( unshare_fs )
			{
				caller.unshare_fs_info();
			}
			
			if ( unshare_files )
			{
				caller.unshare_fd_map();
			}
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
}

