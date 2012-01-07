/*	========
 *	times.cc
 *	========
 */

// Standard C
#include <time.h>

// POSIX
#include "sys/times.h"

// Genie
#include "Genie/current_process.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
	struct StartTime
	{
		clock_t microseconds;
		
		StartTime() : microseconds( clock() )
		{
		}
	};
	
	static StartTime gStartTime;
	
	
	static clock_t times( struct tms* tp )
	{
		if ( tp != NULL )
		{
			const Times& clocks = current_process().GetTimes();
			
			tp->tms_utime  = clocks.user;
			tp->tms_stime  = clocks.system;
			tp->tms_cutime = clocks.child_user;
			tp->tms_cstime = clocks.child_system;
		}
		
		return clock() - gStartTime.microseconds;
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( times );
	
	#pragma force_active reset
	
}

