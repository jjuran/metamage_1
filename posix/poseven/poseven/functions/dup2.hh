// dup2.hh
// -------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_DUP2_HH
#define POSEVEN_FUNCTIONS_DUP2_HH

// poseven
#include "poseven/types/fd_t.hh"


namespace poseven
{
	
	void dup2( fd_t old_fd, fd_t new_fd );
	
}

#endif

