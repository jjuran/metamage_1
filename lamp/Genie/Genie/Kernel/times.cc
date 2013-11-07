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
			*tp = current_process().GetTimes();
		}
		
		return clock() - gStartTime.microseconds;
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( times );
	
	#pragma force_active reset
	
}

