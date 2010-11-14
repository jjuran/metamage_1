/*	==========================
 *	UnimplementedSystemCall.cc
 *	==========================
 */

#include "Genie/Dispatch/UnimplementedSystemCall.hh"

// Standard C
#include <errno.h>

// Genie
#include "Genie/current_process.hh"


namespace Genie
{
	
	int UnimplementedSystemCall()
	{
		return set_errno( ENOSYS );
	}
	
}

