/*
	ftruncate.cc
	------------
*/

#include "poseven/functions/ftruncate.hh"

// POSIX
#include <unistd.h>

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	void ftruncate( fd_t fd, off_t offset )
	{
		throw_posix_result( ::ftruncate( fd, offset ) );
	}
	
}

