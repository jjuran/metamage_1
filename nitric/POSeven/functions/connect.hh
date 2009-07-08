// connect.hh
// ----------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_CONNECT_HH
#define POSEVEN_FUNCTIONS_CONNECT_HH

// POSIX
#include <unistd.h>

// POSeven
#include "POSeven/types/errno_t.hh"
#include "POSeven/types/fd_t.hh"
#include "POSeven/types/sockaddr.hh"


namespace poseven
{
	
	template < address_family af >
	inline void connect( fd_t sock, const typename sockaddr_traits< af >::address_type& server_address )
	{
		throw_posix_result( ::connect( sock,
		                               (const sockaddr*) &server_address,
		                               sizeof (typename sockaddr_traits< af >::address_type) ) );
	}
	
}

#endif

