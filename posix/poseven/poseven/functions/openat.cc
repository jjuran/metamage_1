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

// Convergence
#include "at.h"


namespace poseven
{
	
	Nucleus::Owned< fd_t > openat( fd_t dirfd, const char* path, open_flags_t flags, mode_t mode )
	{
		// register errnos
		
		int fd = throw_posix_result( ::openat( dirfd, path, flags, mode ) );
		
		return Nucleus::Owned< fd_t >::Seize( fd_t( fd ) );
	}
	
}

