/*
	fstat.cc
	--------
*/

#include "poseven/functions/fstat.hh"

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	struct stat fstat( fd_t fd )
	{
		struct stat stat_buffer;
		
		throw_posix_result( ::fstat( fd, &stat_buffer ) );
		
		return stat_buffer;
	}
	
}

