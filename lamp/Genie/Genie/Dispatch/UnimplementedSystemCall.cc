/*	==========================
 *	UnimplementedSystemCall.cc
 *	==========================
 */

#include "Genie/Dispatch/UnimplementedSystemCall.hh"

// Standard C
#include <errno.h>

// Genie
#include "Genie/current_process.hh"
#include "Genie/SystemCalls.hh"


namespace Genie
{
	
	int UnimplementedSystemCall()
	{
		SystemCallFrame frame( "* UNIMPLEMENTED *" );
		
		return set_errno( ENOSYS );
	}
	
}

