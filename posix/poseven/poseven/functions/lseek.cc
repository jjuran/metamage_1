/*
	lseek.cc
	--------
*/

#include "poseven/functions/lseek.hh"

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	off_t lseek( fd_t fd, off_t offset, seek_t whence )
	{
		return throw_posix_result( ::lseek( fd, offset, whence ) );
	}
	
}

