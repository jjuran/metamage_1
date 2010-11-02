/*
	unlinkat.cc
	-----------
*/

#include "poseven/functions/unlinkat.hh"

// POSIX
#include <unistd.h>

// Extended API Set Part 2
#include "extended-api-set/part-2.h"

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	void unlinkat( fd_t         dirfd,
	               const char*  path,
	               at_flags_t   flags )
	{
		throw_posix_result( ::unlinkat( dirfd, path, flags ) );
	}
	
}

