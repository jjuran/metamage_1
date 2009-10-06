// fchmod.hh
// ---------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_FCHMOD_HH
#define POSEVEN_FUNCTIONS_FCHMOD_HH

// poseven
#include "poseven/types/errno_t.hh"
#include "poseven/types/fd_t.hh"
#include "poseven/types/mode_t.hh"


namespace poseven
{
	
	inline void fchmod( fd_t fd, mode_t mode )
	{
		throw_posix_result( ::fchmod( fd, mode ) );
	}
	
}

#endif

