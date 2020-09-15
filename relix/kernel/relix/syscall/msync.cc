/*
	msync.cc
	--------
*/

#include "relix/syscall/msync.hh"

// relix
#include "relix/api/current_process.hh"
#include "relix/api/errno.hh"
#include "relix/task/process.hh"
#include "relix/task/process_image.hh"


namespace relix
{
	
	int msync( void* addr, unsigned long len, int flags )
	{
		try
		{
			current_process().get_process_image().msync_memory_mapping( addr, len, flags );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
}
