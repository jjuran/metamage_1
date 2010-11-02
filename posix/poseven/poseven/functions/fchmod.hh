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
#include "poseven/types/fd_t.hh"
#include "poseven/types/mode_t.hh"


namespace poseven
{
	
	void fchmod( fd_t fd, mode_t mode );
	
}

#endif

