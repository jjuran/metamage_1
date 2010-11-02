/*
	chmod.cc
	--------
*/

#include "poseven/functions/chmod.hh"

// POSIX
#include <sys/stat.h>

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	void chmod( const char* pathname, mode_t mode )
	{
		throw_posix_result( ::chmod( pathname, mode ) );
	}
	
}

