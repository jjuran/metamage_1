/*	========
 *	ioctl.cc
 *	========
 */

// Standard C
#include <errno.h>

// POSIX
#include "sys/ioctl.h"

// Nitrogen / Carbon
#include "Nitrogen/OSStatus.h"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"

// Genie
#include "Genie/IO/Stream.hh"
#include "Genie/pathnames.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	static int ioctl( int filedes, unsigned long request, int* argp )
	{
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
					
					return 0;
				
				case FIONREAD:
					// not implemented
					break;
				
				default:
					stream.IOCtl( request, argp );
					return 0;
			}
		}
		catch ( ... )
		{
		}
		
		return CurrentProcess().SetErrno( EINVAL );
	}
	
	REGISTER_SYSTEM_CALL( ioctl );
	
}

