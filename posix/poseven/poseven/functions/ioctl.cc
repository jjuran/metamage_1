/*
	ioctl.cc
	--------
*/

#include "poseven/functions/ioctl.hh"

// POSIX
#include <sys/ioctl.h>

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	void ioctl( fd_t fd, unsigned long command, void* argp )
	{
		throw_posix_result( ::ioctl( fd, command, argp ) );
	}
	
}

