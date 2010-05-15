// read.hh
// -------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_READ_HH
#define POSEVEN_FUNCTIONS_READ_HH

// POSIX
#include <unistd.h>

// poseven
#include "poseven/types/errno_t.hh"
#include "poseven/types/fd_t.hh"


namespace poseven
{
	
	inline ssize_t read( fd_t fd, char* buffer, size_t bytes_requested )
	{
		return throw_posix_result( ::read( fd, buffer, bytes_requested ) );
	}
	
}

#endif

