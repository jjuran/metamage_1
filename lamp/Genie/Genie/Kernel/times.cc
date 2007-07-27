/*	========
 *	times.cc
 *	========
 */

// Standard C++
#include <algorithm>

// POSIX
#include "sys/times.h"

// Nucleus
#include "Nucleus/NAssert.h"

// Genie
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	DECLARE_MODULE_INIT( Kernel_times )
	DEFINE_MODULE_INIT(  Kernel_times )
	
	
	static clock_t times( struct tms* tp )
	{
		if ( tp != NULL )
		{
			*tp = CurrentProcess().GetTimes();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( times );
	
}

