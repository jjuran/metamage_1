/*
	nanosleep.cc
	------------
*/

#include "relix/syscall/nanosleep.hh"

// POSIX
#include <sys/time.h>

// Standard C
#include <errno.h>
#include <stdint.h>
#include <time.h>

// relix-kernel
#include "relix/api/errno.hh"
#include "relix/api/request_timed_wakeup.hh"
#include "relix/api/try_again.hh"
#include "relix/signal/signal.hh"
#include "relix/syscall/registry.hh"


#ifndef NANOSLEEP_DOZE
#define NANOSLEEP_DOZE  (-1)
#endif


namespace relix
{
	
	static inline
	uint64_t microseconds_from_timespec( const struct timespec& time )
	{
		return time.tv_sec * 1000000ull + time.tv_nsec / 1000;
	}
	
	static inline
	void set_timespec_microseconds( struct timespec& ts, uint64_t microseconds )
	{
		ts.tv_sec  = microseconds / 1000000;
		ts.tv_nsec = microseconds % 1000000 * 1000;
	}
	
	static inline
	void set_timespec_microseconds( struct timespec* ts, uint64_t microseconds )
	{
		if ( ts != NULL )
		{
			set_timespec_microseconds( *ts, microseconds );
		}
	}
	
	int nanosleep( const timespec* requested, timespec* remaining )
	{
		if ( requested == NULL )
		{
			return set_errno( EFAULT );
		}
		
		const bool dozing = requested->tv_nsec == NANOSLEEP_DOZE;
		
		const struct timespec& minimum = requested[ dozing ? 1 : 0 ];
		const struct timespec& maximum = requested[ dozing ? 2 : 0 ];
		
		const uint64_t min_microseconds = microseconds_from_timespec( minimum );
		const uint64_t max_microseconds = microseconds_from_timespec( maximum );
		
		const int64_t delta_microseconds = max_microseconds - min_microseconds;
		
		const uint64_t start_microseconds = clock();
		
		const uint64_t end_microseconds = start_microseconds + max_microseconds;
		
		int64_t remaining_microseconds = max_microseconds;
		
		int result = 0;
		
		try
		{
			// Yield at least once, even for 0 seconds
			do
			{
				request_timed_wakeup( remaining_microseconds * 1000 );
				
				try_again( false );
				
				remaining_microseconds = end_microseconds - clock();
			}
			while ( remaining_microseconds > delta_microseconds );
		}
		catch ( ... )
		{
			remaining_microseconds = end_microseconds - clock();
			
			result = set_errno_from_exception();
			
			prevent_syscall_restart();
		}
		
		if ( remaining_microseconds < 0 )
		{
			remaining_microseconds = 0;
		}
		
		set_timespec_microseconds( remaining, remaining_microseconds );
		
		return result;
	}
	
}
