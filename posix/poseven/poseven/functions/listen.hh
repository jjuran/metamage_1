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

// POSIX
#include <sys/socket.h>

// poseven
#include "poseven/types/errno_t.hh"
#include "poseven/types/fd_t.hh"


namespace poseven
{
	
	inline void listen( fd_t fd, int backlog )
	{
		throw_posix_result( ::listen( fd, backlog ) );
	}
	
}

#endif

