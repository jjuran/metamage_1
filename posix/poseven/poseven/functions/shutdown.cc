/*
	shutdown.cc
	-----------
*/

#include "poseven/functions/shutdown.hh"

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	void shutdown( fd_t fd, shut_t how )
	{
		throw_posix_result( ::shutdown( fd, how ) );
	}
	
}

