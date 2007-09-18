/*	=======
 *	time.cc
 *	=======
 */

// Standard C++
#include <algorithm>

// POSIX
#include "sys/time.h"

// Nucleus
#include "Nucleus/NAssert.h"

// Nitrogen
#include "Nitrogen/OSUtils.h"
#include "Nitrogen/Timer.h"

// TimeOff
#include "TimeOff.hh"

// Genie
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


namespace Genie
{
	
	DECLARE_MODULE_INIT( Kernel_time )
	DEFINE_MODULE_INIT(  Kernel_time )
	
	namespace N = Nitrogen;
	
	struct StartTime
	{
		UInt64 microseconds;
		UInt32 dateTime;
		
		StartTime() : microseconds( N::Microseconds() ), dateTime( TimeOff::GlobalDateTime() )  {}
	};
	
	static StartTime gStartTime;
	
	
	static int gettimeofday( struct timeval* tv, struct timezone* tz )
	{
		SystemCallFrame frame( "gettimeofday" );
		
		long gmtDelta = TimeOff::GetGMTDelta();
		
		const unsigned long timeDiff = TimeOff::MacToUnixTimeDifference( gmtDelta );
		
		if ( tv != NULL )
		{
			UInt32 now = N::GetDateTime() - timeDiff;
			
			tv->tv_sec  = now;
			tv->tv_usec = (N::Microseconds() - gStartTime.microseconds) % 1000000;
		}
		
		if ( tz != NULL )
		{
			tz->tz_minuteswest = -gmtDelta / 60;
			tz->tz_dsttime     = 0;
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( gettimeofday );
	
}

