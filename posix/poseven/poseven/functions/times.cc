/*
	times.cc
	--------
*/

#include "poseven/functions/times.hh"

// POSIX
#include <sys/times.h>

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	void times( struct tms& time_set )
	{
		throw_posix_result( ::times( &time_set ) );
	}
	
}

