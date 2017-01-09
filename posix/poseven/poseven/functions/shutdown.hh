// shutdown.hh
// -----------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_SHUTDOWN_HH
#define POSEVEN_FUNCTIONS_SHUTDOWN_HH

// POSIX
#ifdef __APPLE__
// This is needed to compile with Mac OS X 10.3's headers.
#include <sys/types.h>
#endif
#include <sys/socket.h>

// poseven
#ifndef POSEVEN_TYPES_FD_T_HH
#include "poseven/types/fd_t.hh"
#endif


namespace poseven
{
	
	enum shut_t
	{
		shut_rd   = SHUT_RD,
		shut_wr   = SHUT_WR,
		shut_rdwr = SHUT_RDWR,
		
		shut_t_max = nucleus::enumeration_traits< int >::max
	};
	
	void shutdown( fd_t fd, shut_t how );
	
}

#endif
