/*
	munmap.cc
	---------
*/

#include "relix/syscall/munmap.hh"

// Standard C
#include <errno.h>

// relix
#include "relix/api/current_process.hh"
#include "relix/api/errno.hh"
#include "relix/task/process.hh"
#include "relix/task/process_image.hh"


namespace relix
{
	
	int munmap( void *addr, unsigned long len )
	{
		if ( len == 0 )
		{
			return set_errno( EINVAL );
		}
		
		try
		{
			current_process().get_process_image().remove_memory_mapping( addr );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
}
