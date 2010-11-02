/*
	symlinkat.cc
	------------
*/

#include "poseven/functions/symlinkat.hh"

// POSIX
#include <unistd.h>

// Extended API Set Part 2
#include "extended-api-set/part-2.h"

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	void symlinkat( const char* target, fd_t dirfd, const char* path )
	{
		throw_posix_result( ::symlinkat( target, dirfd, path ) );
	}
	
}

