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
#include "Genie/Devices.hh"
#include "Genie/FileHandle.hh"
#include "Genie/pathnames.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	static int ioctl( int filedes, unsigned long request, int* argp )
	{
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		switch ( request )
		{
			case FIONBIO:
				if ( *argp )
				{
					files[ filedes ].handle.SetNonBlocking();
				}
				else
				{
					files[ filedes ].handle.SetBlocking();
				}
				
				return 0;
			
			case FIONREAD:
				// not implemented
				break;
			
			default:
				break;
		}
		
		return CurrentProcess().SetErrno( EINVAL );
	}
	
	REGISTER_SYSTEM_CALL( ioctl );
	
}

