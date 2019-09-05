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

// poseven
#ifndef POSEVEN_TYPES_ERRNO_T_HH
#include "poseven/types/errno_t.hh"
#endif
#ifndef POSEVEN_TYPES_FD_T_HH
#include "poseven/types/fd_t.hh"
#endif
#ifndef POSEVEN_TYPES_SOCKADDR_HH
#include "poseven/types/sockaddr.hh"
#endif

#ifdef __APPLE__
#ifndef _SOCKLEN_T
typedef int socklen_t;
#endif
#endif


namespace poseven
{
	
	inline
	void connect( fd_t sock, const sockaddr& server_address, socklen_t size )
	{
		throw_posix_result( ::connect( sock, &server_address, size ) );
	}
	
	template < address_family af >
	void connect( fd_t                                                 sock,
	              const typename sockaddr_traits< af >::address_type&  addr )
	{
		connect( sock, (const sockaddr&) addr, sizeof addr );
	}
	
}

#endif
