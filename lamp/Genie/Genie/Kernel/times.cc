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
#include "Genie/SystemCalls.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	DECLARE_MODULE_INIT( Kernel_times )
	DEFINE_MODULE_INIT(  Kernel_times )
	
	
	static clock_t times( struct tms* tp )
	{
		SystemCallFrame frame( "times" );
		
		if ( tp != NULL )
		{
			const Times& clocks = CurrentProcess().GetTimes();
			
			tp->tms_utime  = clocks.user         * (CLOCKS_PER_SEC / 1000000.0);
			tp->tms_stime  = clocks.system       * (CLOCKS_PER_SEC / 1000000.0);
			tp->tms_cutime = clocks.child_user   * (CLOCKS_PER_SEC / 1000000.0);
			tp->tms_cstime = clocks.child_system * (CLOCKS_PER_SEC / 1000000.0);
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( times );
	
}

