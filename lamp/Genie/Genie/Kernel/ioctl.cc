/*	========
 *	ioctl.cc
 *	========
 */

// Standard C
#include <errno.h>

// POSIX
#include "sys/ioctl.h"

// vfs
#include "vfs/file_descriptor.hh"

// relix-kernel
#include "relix/task/fd_map.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/IO/Base.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
	static int ioctl( int filedes, unsigned long request, int* argp )
	{
		relix::fd_map& files = current_process().FileDescriptors();
		
		try
		{
			vfs::file_descriptor& file = files[ filedes ];
			
			if ( request == FIOCLEX )
			{
				file.set_to_close_on_exec( true );
			}
			else if ( request == FIONCLEX )
			{
				file.set_to_close_on_exec( false );
			}
			else
			{
				file.handle->IOCtl( request, argp );
			}
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( ioctl );
	
	#pragma force_active reset
	
}

