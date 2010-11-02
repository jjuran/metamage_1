/*
	chdir.cc
	--------
*/

#include "poseven/functions/chdir.hh"

// POSIX
#include <unistd.h>

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	void chdir( const char* path )
	{
		throw_posix_result( ::chdir( path ) );
	}
	
}

