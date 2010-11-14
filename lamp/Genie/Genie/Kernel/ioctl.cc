/*	========
 *	ioctl.cc
 *	========
 */

// Standard C
#include <errno.h>

// POSIX
#include "sys/ioctl.h"

// OSErrno
#include "OSErrno/OSErrno.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/FileDescriptor.hh"
#include "Genie/IO/Base.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
	static int ioctl( int filedes, unsigned long request, int* argp )
	{
		fd_table& files = current_process().FileDescriptors();
		
		try
		{
			IOHandle& io = *files[ filedes ].handle;
			
			io.IOCtl( request, argp );
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

