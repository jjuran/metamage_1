/*	=======
 *	time.cc
 *	=======
 */

// Standard C++
#include <algorithm>

// POSIX
#include "errno.h"
#include "sys/time.h"

// Nitrogen
#include "Nitrogen/DateTimeUtils.hh"
#include "Nitrogen/OSUtils.hh"
#include "Nitrogen/Timer.hh"

// TimeOff
#include "TimeOff/TimeOff.hh"

// Pedestal
#include "Pedestal/Application.hh"

// Genie
#include "Genie/caught_signal.hh"
#include "Genie/current_process.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"


#ifndef NANOSLEEP_DOZE
#define NANOSLEEP_DOZE  (-1)
#endif


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace Ped = Pedestal;
	
	
	static inline UInt32 GlobalDateTime()
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
		if ( tv != NULL )
		{
			UInt64 now = N::Microseconds() + gStartTime.diff;
			
			tv->tv_sec  = now / 1000000;
			tv->tv_usec = now % 1000000;
		}
		
		return 0;
	}
	
	
	static inline UInt64 microseconds_from_timespec( const struct timespec& time )
	{
		const UInt64 result = time.tv_sec * 1000000LL + time.tv_nsec / 1000;
		
		return result;
	}
	
	static inline void set_timespec_microseconds( struct timespec& time, UInt64 microseconds )
	{
		time.tv_sec  = microseconds / 1000000;
		time.tv_nsec = microseconds % 1000000 * 1000;
	}
	
	static inline void set_timespec_microseconds( struct timespec* time, UInt64 microseconds )
	{
		if ( time != NULL )
		{
			set_timespec_microseconds( *time, microseconds );
		}
	}
	
	static int nanosleep( const struct timespec* requested, struct timespec* remaining )
	{
		if ( requested == NULL )
		{
			return set_errno( EFAULT );
		}
		
		const bool dozing = requested->tv_nsec == NANOSLEEP_DOZE;
		
		const struct timespec& minimum = requested[ dozing ? 1 : 0 ];
		const struct timespec& maximum = requested[ dozing ? 2 : 0 ];
		
		const UInt64 minimum_microseconds = microseconds_from_timespec( minimum );
		const UInt64 maximum_microseconds = microseconds_from_timespec( maximum );
		
		const SInt64 delta_microseconds = maximum_microseconds - minimum_microseconds;
		
		UInt64 start_microseconds = N::Microseconds();
		
		UInt64 end_microseconds = start_microseconds + maximum_microseconds;
		
		SInt64 remaining_microseconds = maximum_microseconds;
		
		int result = 0;
		
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
			while ( remaining_microseconds > delta_microseconds );
		}
		catch ( ... )
		{
			remaining_microseconds = end_microseconds - N::Microseconds();
			
			result = set_errno_from_exception();
			
			prevent_syscall_restart();
		}
		
		set_timespec_microseconds( remaining, std::max( remaining_microseconds, 0LL ) );
		
		return result;
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( gettimeofday );
	REGISTER_SYSTEM_CALL( nanosleep    );
	
	#pragma force_active reset
	
}

