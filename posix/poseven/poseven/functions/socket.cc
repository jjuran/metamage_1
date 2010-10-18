/*
	socket.cc
	---------
*/

#include "poseven/functions/socket.hh"

// POSIX
#include <sys/socket.h>

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	nucleus::owned< fd_t > socket( protocol_family  domain,
	                               socket_type      type )
	{
		const int fd = throw_posix_result( ::socket( domain, type, 0 ) );
		
		return nucleus::owned< fd_t >::seize( fd_t( fd ) );
	}
	
}

