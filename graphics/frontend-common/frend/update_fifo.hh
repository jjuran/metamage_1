/*
	update_fifo.hh
	--------------
*/

#ifndef FREND_UPDATEFIFO_HH
#define FREND_UPDATEFIFO_HH

// POSIX
#include <fcntl.h>
#include <unistd.h>


#define UPDATE_FIFO  "update-signal.fifo"

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
		return close( open( UPDATE_FIFO, O_WRONLY ) );
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
	return CONFIG_UPDATES_VIA_FIFO ? open( UPDATE_FIFO, O_RDONLY | O_NONBLOCK )
	                               : 0;
}

}

#endif
