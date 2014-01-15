/*
	fd_pair.cc
	----------
*/

#include "poseven/types/fd_pair.hh"

// POSIX
#include <unistd.h>


namespace poseven
{
	
	static inline void close_fd( int fd )
	{
		if ( fd >= 0 )
		{
			::close( fd );
		}
	}
	
	fd_pair::~fd_pair()
	{
		close_fd( its_fds[ 0 ] );
		close_fd( its_fds[ 1 ] );
	}
	
}

