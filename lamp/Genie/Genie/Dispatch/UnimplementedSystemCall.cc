/*	==========================
 *	UnimplementedSystemCall.cc
 *	==========================
 */

#include "Genie/Dispatch/UnimplementedSystemCall.hh"

// Standard C
#include <errno.h>

// Genie
#include "Genie/SystemCalls.hh"


namespace Genie
{
	
	int UnimplementedSystemCall()
	{
		SystemCallFrame frame( "* UNIMPLEMENTED *" );
		
		return frame.SetErrno( ENOSYS );
	}
	
}

