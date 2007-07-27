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
#include "Genie/IO/Stream.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	DECLARE_MODULE_INIT( Kernel_ioctl )
	DEFINE_MODULE_INIT(  Kernel_ioctl )
	
	static int ioctl( int filedes, unsigned long request, int* argp )
	{
		SystemCallFrame frame( "ioctl" );
		
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		try
		{
			StreamHandle& stream = IOHandle_Cast< StreamHandle >( *files[ filedes ].handle );
			
			switch ( request )
			{
				case FIONBIO:
					if ( *argp )
					{
						stream.SetNonBlocking();
					}
					else
					{
						stream.SetBlocking();
					}
					
					break;
				
				case FIONREAD:
					// not implemented
					return CurrentProcess().SetErrno( EINVAL );
				
				default:
					stream.IOCtl( request, argp );
					break;
			}
		}
		catch ( ... )
		{
			return GetErrnoFromExceptionInSystemCall();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( ioctl );
	
}

