/*
	symlink.cc
	----------
*/

#include "poseven/functions/symlink.hh"

// POSIX
#include <unistd.h>

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	void symlink( const char* from, const char* to )
	{
		throw_posix_result( ::symlink( from, to ) );
	}
	
}

