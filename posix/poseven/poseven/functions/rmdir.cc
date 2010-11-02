/*
	rmdir.cc
	--------
*/

#include "poseven/functions/rmdir.hh"

// POSIX
#include <unistd.h>

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	void rmdir( const char* path )
	{
		throw_posix_result( ::rmdir( path ) );
	}
	
}

