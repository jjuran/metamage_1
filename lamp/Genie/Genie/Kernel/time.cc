/*	=======
 *	time.cc
 *	=======
 */

// Standard C
#include <stdint.h>
#include <time.h>

// POSIX
#include "errno.h"
#include "sys/time.h"

// relix-kernel
#include "relix/api/request_timed_wakeup.hh"
#include "relix/api/try_again.hh"
#include "relix/signal/signal.hh"
#include "relix/syscall/registry.hh"

// Genie
#include "Genie/current_process.hh"


#ifndef NANOSLEEP_DOZE
#define NANOSLEEP_DOZE  (-1)
#endif


namespace Genie
{
	
	static inline uint64_t microseconds_from_timespec( const struct timespec& time )
	{
		const uint64_t result = time.tv_sec * 1000000LL + time.tv_nsec / 1000;
		
		return result;
	}
	
	static inline void set_timespec_microseconds( struct timespec& time, uint64_t microseconds )
	{
		time.tv_sec  = microseconds / 1000000;
		time.tv_nsec = microseconds % 1000000 * 1000;
	}
	
	static inline void set_timespec_microseconds( struct timespec* time, uint64_t microseconds )
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
		
		const uint64_t minimum_microseconds = microseconds_from_timespec( minimum );
		const uint64_t maximum_microseconds = microseconds_from_timespec( maximum );
		
		const int64_t delta_microseconds = maximum_microseconds - minimum_microseconds;
		
		uint64_t start_microseconds = clock();
		
		uint64_t end_microseconds = start_microseconds + maximum_microseconds;
		
		int64_t remaining_microseconds = maximum_microseconds;
		
		int result = 0;
		
		try
		{
			// Yield at least once, even for 0 seconds
			do
			{
				relix::request_timed_wakeup( remaining_microseconds * 1000 );
				
				relix::try_again( false );
				
				remaining_microseconds = end_microseconds - clock();
			}
			while ( remaining_microseconds > delta_microseconds );
		}
		catch ( ... )
		{
			remaining_microseconds = end_microseconds - clock();
			
			result = set_errno_from_exception();
			
			relix::prevent_syscall_restart();
		}
		
		if ( remaining_microseconds < 0 )
		{
			remaining_microseconds = 0;
		}
		
		set_timespec_microseconds( remaining, remaining_microseconds );
		
		return result;
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( gettimeofday );
	REGISTER_SYSTEM_CALL( nanosleep    );
	
	#pragma force_active reset
	
}
