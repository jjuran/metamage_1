// mkdirat.hh
// ----------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_MKDIRAT_HH
#define POSEVEN_FUNCTIONS_MKDIRAT_HH

// POSIX
#include <sys/stat.h>

// poseven
#include "poseven/types/errno_t.hh"
#include "poseven/types/mode_t.hh"


namespace poseven
{
	
	inline void mkdirat( fd_t dirfd, const char* path, mode_t mode = _777 )
	{
		throw_posix_result( ::mkdirat( dirfd, path, mode ) );
	}
	
}

#endif

