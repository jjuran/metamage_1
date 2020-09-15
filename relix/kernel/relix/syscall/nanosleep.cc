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
	uint64_t nanoseconds_from_timespec( const struct timespec& time )
	{
		return time.tv_sec * 1000000000ull + time.tv_nsec;
	}
	
	static inline
	void set_timespec_nanoseconds( struct timespec& time, uint64_t nanoseconds )
	{
		time.tv_sec  = nanoseconds / 1000000000;
		time.tv_nsec = nanoseconds % 1000000000;
	}
	
	static inline
	void set_timespec_nanoseconds( struct timespec* time, uint64_t nanoseconds )
	{
		if ( time != NULL )
		{
			set_timespec_nanoseconds( *time, nanoseconds );
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
		
		const uint64_t min_nanoseconds = nanoseconds_from_timespec( minimum );
		const uint64_t max_nanoseconds = nanoseconds_from_timespec( maximum );
		
		const int64_t delta_nanoseconds = max_nanoseconds - min_nanoseconds;
		
		const uint64_t start_nanoseconds = clock() * 1000;
		
		const uint64_t end_nanoseconds = start_nanoseconds + max_nanoseconds;
		
		int64_t remaining_nanoseconds = max_nanoseconds;
		
		int result = 0;
		
		try
		{
			// Yield at least once, even for 0 seconds
			do
			{
				request_timed_wakeup( remaining_nanoseconds );
				
				try_again( false );
				
				remaining_nanoseconds = end_nanoseconds - clock() * 1000;
			}
			while ( remaining_nanoseconds > delta_nanoseconds );
		}
		catch ( ... )
		{
			remaining_nanoseconds = end_nanoseconds - clock() * 1000;
			
			result = set_errno_from_exception();
			
			prevent_syscall_restart();
		}
		
		if ( remaining_nanoseconds < 0 )
		{
			remaining_nanoseconds = 0;
		}
		
		set_timespec_nanoseconds( remaining, remaining_nanoseconds );
		
		return result;
	}
	
}
