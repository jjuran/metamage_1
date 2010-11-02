/*
	accept.cc
	---------
*/

#include "poseven/functions/accept.hh"

// POSIX
#include <unistd.h>

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	nucleus::owned< fd_t > accept( fd_t        listener,
	                               sockaddr*   address,
	                               socklen_t*  length )
	{
		int result = throw_posix_result( ::accept( listener,
		                                           address,
		                                           length ) );
		
		return nucleus::owned< fd_t >::seize( fd_t( result ) );
	}
	
}

