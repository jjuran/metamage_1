// ftruncate.hh
// ------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_FTRUNCATE_HH
#define POSEVEN_FUNCTIONS_FTRUNCATE_HH

// POSIX
#include <unistd.h>

// POSeven
#include "POSeven/Errno.hh"
#include "POSeven/types/fd_t.hh"


namespace poseven
{
	
	inline void ftruncate( fd_t fd, off_t offset )
	{
		throw_posix_result( ::ftruncate( fd, offset ) );
	}
	
}

#endif

