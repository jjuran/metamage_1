/*
	fchmod.cc
	---------
*/

#include "poseven/functions/fchmod.hh"

// POSIX
#include <sys/stat.h>

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	void fchmod( fd_t fd, mode_t mode )
	{
		throw_posix_result( ::fchmod( fd, mode ) );
	}
	
}

