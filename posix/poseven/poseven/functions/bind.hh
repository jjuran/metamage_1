// bind.hh
// -------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_BIND_HH
#define POSEVEN_FUNCTIONS_BIND_HH

// poseven
#include "poseven/types/errno_t.hh"
#include "poseven/types/fd_t.hh"
#include "poseven/types/sockaddr.hh"


namespace poseven
{
	
	template < address_family af >
	inline void bind( fd_t fd, const typename sockaddr_traits< af >::address_type& address )
	{
		throw_posix_result( ::bind( fd,
		                            (const sockaddr*) &address,
		                            sizeof (typename sockaddr_traits< af >::address_type) ) );
	}
	
}

#endif

