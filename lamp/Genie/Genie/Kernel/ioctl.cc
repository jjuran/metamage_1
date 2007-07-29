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
#include "Genie/IO/Base.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


namespace Genie
{
	
	DECLARE_MODULE_INIT( Kernel_ioctl )
	DEFINE_MODULE_INIT(  Kernel_ioctl )
	
	static int ioctl( int filedes, unsigned long request, int* argp )
	{
		SystemCallFrame frame( "ioctl" );
		
		FileDescriptorMap& files = frame.Caller().FileDescriptors();
		
		try
		{
			IOHandle& io = *files[ filedes ].handle;
			
			io.IOCtl( request, argp );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( ioctl );
	
}

