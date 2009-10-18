/*	=======
 *	time.cc
 *	=======
 */

// Standard C++
#include <algorithm>

// POSIX
#include "sys/time.h"

// Nitrogen
#include "Nitrogen/OSUtils.h"
#include "Nitrogen/Timer.h"

// TimeOff
#include "TimeOff.hh"

// Pedestal
#include "Pedestal/Application.hh"

// Genie
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace Ped = Pedestal;
	
	
	inline UInt32 GlobalDateTime()
	{
		return N::GetDateTime() - TimeOff::GetGMTDelta();
	}
	
	struct StartTime
	{
		UInt64 microseconds;
		UInt32 dateTime;
		time_t unixTime;
		UInt64 diff;
		
		StartTime() : microseconds( N::Microseconds() ),
		              dateTime    ( GlobalDateTime()  ),
		              unixTime    ( dateTime - TimeOff::MacUnixEpochOffset() ),
		              diff        ( unixTime * 1000000ULL - microseconds )
		{
		}
	};
	
	static StartTime gStartTime;
	
	
	static int gettimeofday( struct timeval* tv, struct timezone* tz )
	{
		SystemCallFrame frame( "gettimeofday" );
		
		if ( tv != NULL )
		{
			UInt64 now = N::Microseconds() + gStartTime.diff;
			
			tv->tv_sec  = now / 1000000;
			tv->tv_usec = now % 1000000;
		}
		
		return 0;
	}
	
	
	static int nanosleep( const struct timespec* requested, struct timespec* remaining )
	{
		SystemCallFrame frame( "nanosleep" );
		
		if ( requested == NULL )
		{
			return frame.SetErrno( EFAULT );
		}
		
		UInt64 start_microseconds = N::Microseconds();
		
		SInt64 remaining_microseconds = requested->tv_sec * 1000000LL + requested->tv_nsec / 1000;
		
		UInt64 end_microseconds = start_microseconds + remaining_microseconds;
		
		try
		{
			// Yield at least once, even for 0 seconds
			do
			{
				// Ticks are exactly 1/60 second in OS X, but not in OS 9.
				// Here we pass the number of OS X ticks remaining.
				// The number of OS 9 ticks remaining is slightly larger,
				// since OS 9 ticks are slightly smaller and a few more of them are
				// needed to fill a certain length of time.
				// So our delay will be short-changed, but that's okay because
				// we keep recomputing it, so as remaining_microseconds approaches
				// zero, the error becomes insignificant.
				// And we keep looping until remaining_microseconds becomes zero
				// anyway.
				Ped::AdjustSleepForTimer( remaining_microseconds * 60 / 1000000 );
				
				Yield( kInterruptAlways );  // don't restart sleep
				
				remaining_microseconds = end_microseconds - N::Microseconds();
			}
			while ( remaining_microseconds > 0 );
		}
		catch ( ... )
		{
			if ( remaining != NULL )
			{
				remaining->tv_sec  = remaining_microseconds / 1000000;
				remaining->tv_nsec = remaining_microseconds % 1000000 * 1000;
			}
			
			return frame.SetErrnoFromException();
		}
		
		if ( remaining != NULL )
		{
			remaining->tv_sec  =
			remaining->tv_nsec = 0;
		}
		
		return 0;
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( gettimeofday );
	REGISTER_SYSTEM_CALL( nanosleep    );
	
	#pragma force_active reset
	
}

