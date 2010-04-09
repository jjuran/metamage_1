// openat.hh
// ---------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2006-2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_OPENAT_HH
#define POSEVEN_FUNCTIONS_OPENAT_HH

// Standard C++
#include <string>

// POSIX
#include <fcntl.h>

// poseven
#include "poseven/functions/close.hh"
#include "poseven/types/mode_t.hh"
#include "poseven/types/open_flags_t.hh"


namespace poseven
{
	
	nucleus::owned< fd_t > openat( fd_t dirfd, const char* path, open_flags_t flags, mode_t mode = _666 );
	
	inline nucleus::owned< fd_t > openat( fd_t dirfd, const std::string& path, open_flags_t flags, mode_t mode = _666 )
	{
		return openat( dirfd, path.c_str(), flags, mode );
	}
	
}

#endif

