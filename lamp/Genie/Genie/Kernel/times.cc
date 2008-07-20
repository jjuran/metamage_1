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
	
	DECLARE_MODULE_INIT( Kernel_times )
	DEFINE_MODULE_INIT(  Kernel_times )
	
	
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
			
			tp->tms_utime  = clocks.user         * (CLOCKS_PER_SEC / 1000000.0);
			tp->tms_stime  = clocks.system       * (CLOCKS_PER_SEC / 1000000.0);
			tp->tms_cutime = clocks.child_user   * (CLOCKS_PER_SEC / 1000000.0);
			tp->tms_cstime = clocks.child_system * (CLOCKS_PER_SEC / 1000000.0);
		}
		
		return (N::Microseconds() - gStartTime.microseconds) * (CLOCKS_PER_SEC / 1000000.0);
	}
	
	REGISTER_SYSTEM_CALL( times );
	
}

