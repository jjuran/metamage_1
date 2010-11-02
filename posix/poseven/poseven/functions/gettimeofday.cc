/*
	gettimeofday.cc
	---------------
*/

#include "poseven/functions/gettimeofday.hh"

// POSIX
#include <time.h>

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	void gettimeofday( struct timeval& tv )
	{
		throw_posix_result( ::gettimeofday( &tv, NULL ) );
	}
	
}

