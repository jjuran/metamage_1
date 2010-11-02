/*
	write.cc
	--------
*/

#include "poseven/functions/write.hh"

// POSIX
#include <unistd.h>

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	ssize_t write( fd_t fd, const char* buffer, size_t bytes_requested )
	{
		return throw_posix_result( ::write( fd, buffer, bytes_requested ) );
	}
	
}

