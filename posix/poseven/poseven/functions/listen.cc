/*
	listen.cc
	---------
*/

#include "poseven/functions/listen.hh"

// POSIX
#include <sys/socket.h>

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	void listen( fd_t fd, int backlog )
	{
		throw_posix_result( ::listen( fd, backlog ) );
	}
	
}

