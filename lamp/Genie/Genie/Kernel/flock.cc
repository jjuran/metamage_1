/*	========
 *	flock.cc
 *	========
 */

// Standard C
#include <errno.h>

// POSIX
#include "sys/file.h"

// Genie
#include "Genie/current_process.hh"
//#include "Genie/FileDescriptors.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


namespace Genie
{
	
	static int flock( int fd, int operation )
	{
		SystemCallFrame frame( "flock" );
		
		int non_blocking = operation & LOCK_NB;
		
		operation -= non_blocking;
		
		try
		{
			switch ( operation )
			{
				case LOCK_SH:
				case LOCK_EX:
				case LOCK_UN:
				
				default:
					break;
			}
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return set_errno( EINVAL );
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( flock );
	
	#pragma force_active reset
	
}

