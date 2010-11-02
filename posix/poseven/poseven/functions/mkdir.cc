/*
	mkdir.cc
	--------
*/

#include "poseven/functions/mkdir.hh"

// POSIX
#include <unistd.h>

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	void mkdir( const char* path, mode_t mode )
	{
		throw_posix_result( ::mkdir( path, mode ) );
	}
	
}

