/*
	dup.cc
	------
*/


#include "poseven/functions/dup.hh"

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	nucleus::owned< fd_t > dup( fd_t old_fd )
	{
		const fd_t new_fd = fd_t( throw_posix_result( ::dup( old_fd ) ) );
		
		return nucleus::owned< fd_t >::seize( new_fd );
	}
	
}

