/*	========
 *	times.cc
 *	========
 */

// POSIX
#include "sys/times.h"

// Nitrogen
#include "Nitrogen/Timer.h"

// Genie
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


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
		SystemCallFrame frame( "times" );
		
		if ( tp != NULL )
		{
			const Times& clocks = frame.Caller().GetTimes();
			
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

