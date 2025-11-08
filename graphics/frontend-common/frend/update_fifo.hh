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
