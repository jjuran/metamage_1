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

// Standard C++
#include <string>

// POSIX
#include <sys/stat.h>

// POSeven
#include "POSeven/Errno.hh"
#include "POSeven/types/mode_t.hh"


namespace poseven
{
	
	inline void mkdirat( fd_t dirfd, const char* path, mode_t mode = mode_t( 0777 ) )
	{
		throw_posix_result( ::mkdirat( dirfd, path, mode ) );
	}
	
	inline void mkdirat( fd_t dirfd, const std::string& path, mode_t mode = mode_t( 0777 ) )
	{
		mkdirat( dirfd, path.c_str(), mode );
	}
	
}

#endif

