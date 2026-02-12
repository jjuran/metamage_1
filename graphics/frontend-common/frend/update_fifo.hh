/*
	update_fifo.hh
	--------------
*/

#ifndef FREND_UPDATEFIFO_HH
#define FREND_UPDATEFIFO_HH

// POSIX
#include <fcntl.h>
#include <unistd.h>

// compat
#include "clock/time.h"

// frontend-common
#include "frend/displayfs.hh"

#if defined(__MSYS__)  ||  defined(__CYGWIN)
#define CONFIG_UPDATES_VIA_FIFO  0
#endif

/*
	Signaling updates via a FIFO does work in Mac OS X on PPC,
	but it's unusably slow -- the latency introduced is far worse
	than any tearing that might be observable otherwise.
	
	The problem has been observed occurring in Mac OS X 10.2 through 10.4
	on PPC, and not occurring in 10.5 on PPC nor any OS X version on x86.
*/

#if defined(__APPLE__)  &&  defined(__POWERPC__)
#include <AvailabilityMacros.h>
#ifndef MAC_OS_X_VERSION_10_5
#define CONFIG_UPDATES_VIA_FIFO  0
#endif
#endif

#ifndef CONFIG_UPDATES_VIA_FIFO
#define CONFIG_UPDATES_VIA_FIFO  1
#endif


namespace frend
{

enum
{
	half_tick   = 1000000 / 120,  // 1s/120, or 8333us
	eighth_tick = half_tick / 4,  // 1s/480, or 2083us
	
	min_sleep  = eighth_tick,
};

const int64_t billion = 1000 * 1000 * 1000;

inline
int64_t timespec_diff_us( const timespec& a, const timespec& b )
{
	int64_t dt = (a.tv_sec - b.tv_sec) * billion
	           + a.tv_nsec
	           - b.tv_nsec;
	
	return dt / 1000;
}

inline
int wait_for_update()
{
	if ( CONFIG_UPDATES_VIA_FIFO )
	{
		return close( open( update_fifo, O_WRONLY ) );
	}
	
	usleep( half_tick );
	
	return 0;
}

inline
long wait_for_update_throttled()
{
	wait_for_update();
	
	if ( CONFIG_UPDATES_VIA_FIFO )
	{
		static
		timespec last_time;
		timespec this_time;
		
		clock_gettime( CLOCK_MONOTONIC, &this_time );
		
		int64_t delta = timespec_diff_us( this_time, last_time );
		int64_t sleep = min_sleep - delta;
		
		last_time = this_time;
		
		return sleep > 0 ? sleep : 0;
	}
	
	return 0;
}

inline
int unblock_update_waiters()
{
	return CONFIG_UPDATES_VIA_FIFO ? open( update_fifo, O_RDONLY | O_NONBLOCK )
	                               : 0;
}

}

#endif
