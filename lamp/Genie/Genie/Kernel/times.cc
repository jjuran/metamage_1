/*	========
 *	times.cc
 *	========
 */

// POSIX
#include "sys/times.h"

// Nitrogen
#include "Nitrogen/Timer.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	struct StartTime
	{
		UInt64 microseconds;
		
		StartTime() : microseconds( N::Microseconds() )  {}
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
		
		return N::Microseconds() - gStartTime.microseconds;
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( times );
	
	#pragma force_active reset
	
}

