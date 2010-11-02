/*
	pwrite.cc
	---------
*/

#include "poseven/functions/pwrite.hh"

// POSIX
#include <unistd.h>

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	ssize_t pwrite( fd_t fd, const char* buffer, size_t n_bytes, off_t offset )
	{
		return throw_posix_result( ::pwrite( fd, buffer, n_bytes, offset ) );
	}
	
}

