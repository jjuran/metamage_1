/*
	pread.cc
	--------
*/

#include "poseven/functions/pread.hh"

// POSIX
#include <unistd.h>

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	ssize_t pread( fd_t fd, char* buffer, size_t n_bytes, off_t offset )
	{
		return throw_posix_result( ::pread( fd, buffer, n_bytes, offset ) );
	}
	
}

