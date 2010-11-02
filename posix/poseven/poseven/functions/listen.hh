// listen.hh
// ---------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_LISTEN_HH
#define POSEVEN_FUNCTIONS_LISTEN_HH

// poseven
#include "poseven/types/fd_t.hh"


namespace poseven
{
	
	void listen( fd_t fd, int backlog );
	
}

#endif

