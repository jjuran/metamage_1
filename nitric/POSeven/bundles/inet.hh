// inet.hh
// -------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_BUNDLES_INET_HH
#define POSEVEN_BUNDLES_INET_HH

// POSeven
#include "POSeven/functions/connect.hh"
#include "POSeven/types/sockaddr_in.hh"


namespace poseven
{
	
	inline void connect( fd_t sock, in_addr_t addr, in_port_t port )
	{
		connect< af_inet >( sock, sockaddr_traits< af_inet >::make( addr, port ) );
	}
	
}

#endif

