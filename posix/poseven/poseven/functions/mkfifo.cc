/*
	mkfifo.cc
	---------
*/

#include "poseven/functions/mkfifo.hh"

// POSIX
#include <unistd.h>

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	void mkfifo( const char* pathname, mode_t mode )
	{
		throw_posix_result( ::mkfifo( pathname, mode ) );
	}
	
}

