// openat.cc
// ---------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2006-2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.

#include "poseven/functions/openat.hh"

// Extended API Set Part 2
#include "extended-api-set/part-2.h"


namespace poseven
{
	
	nucleus::owned< fd_t > openat( fd_t dirfd, const char* path, open_flags_t flags, mode_t mode )
	{
		// register errnos
		
		int fd = throw_posix_result( ::openat( dirfd, path, flags, mode ) );
		
		return nucleus::owned< fd_t >::seize( fd_t( fd ) );
	}
	
}

