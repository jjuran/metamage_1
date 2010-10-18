/*
	unlink.cc
	---------
*/

#include "poseven/functions/unlink.hh"

// POSIX
#include <unistd.h>

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	void unlink( const char* path )
	{
		throw_posix_result( ::unlink( path ) );
	}
	
}

