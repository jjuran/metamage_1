/*
	socketpair.cc
	-------------
*/

#include "poseven/functions/socketpair.hh"

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	fd_pair socketpair( protocol_family  domain,
	                    socket_type      type )
	{
		int fds[ 2 ];
		
		throw_posix_result( ::socketpair( domain, type, 0, fds ) );
		
		return fd_pair( fds );
	}
	
}
