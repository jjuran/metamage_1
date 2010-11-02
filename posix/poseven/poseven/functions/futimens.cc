/*
	futimens.cc
	-----------
*/

#include "poseven/functions/futimens.hh"

// Extended API Set Part 2
#include "extended-api-set/part-2.h"

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	void futimens( fd_t fd, const timespec times[2] )
	{
		throw_posix_result( ::futimens( fd, times ) );
	}
	
}

