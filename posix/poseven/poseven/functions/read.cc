/*
	read.cc
	-------
*/

#include "poseven/functions/read.hh"

// POSIX
#include <unistd.h>

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	ssize_t read( fd_t fd, char* buffer, size_t bytes_requested )
	{
		return throw_posix_result( ::read( fd, buffer, bytes_requested ) );
	}
	
}

