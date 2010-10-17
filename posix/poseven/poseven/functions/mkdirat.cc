/*
	mkdirat.cc
	----------
*/

#include "poseven/functions/mkdirat.hh"

// POSIX
#include <sys/stat.h>

// Extended API Set Part 2
#include "extended-api-set/part-2.h"

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	void mkdirat( fd_t dirfd, const char* path, mode_t mode )
	{
		throw_posix_result( ::mkdirat( dirfd, path, mode ) );
	}
	
}

