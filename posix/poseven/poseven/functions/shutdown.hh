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
#include <sys/socket.h>

// poseven
#include "poseven/types/errno_t.hh"
#include "poseven/types/fd_t.hh"


namespace poseven
{
	
	enum shut_t
	{
		shut_rd   = SHUT_RD,
		shut_wr   = SHUT_WR,
		shut_rdwr = SHUT_RDWR,
		
		shut_t_max = nucleus::enumeration_traits< int >::max
	};
	
	inline void shutdown( fd_t fd, shut_t how )
	{
		throw_posix_result( ::shutdown( fd, how ) );
	}
	
}

#endif

