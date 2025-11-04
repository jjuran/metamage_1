/*
	update_fifo.hh
	--------------
*/

#ifndef FREND_UPDATEFIFO_HH
#define FREND_UPDATEFIFO_HH

// POSIX
#include <fcntl.h>
#include <unistd.h>

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

inline
int wait_for_update()
{
	if ( CONFIG_UPDATES_VIA_FIFO )
	{
		return close( open( update_fifo, O_WRONLY ) );
	}
	
	enum
	{
		half_tick = 1000000 / 120,  // 1s/120, or 8333us
	};
	
	usleep( half_tick );
	
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
