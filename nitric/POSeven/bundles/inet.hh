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
#include "POSeven/functions/accept.hh"
#include "POSeven/functions/bind.hh"
#include "POSeven/functions/connect.hh"
#include "POSeven/functions/socket.hh"
#include "POSeven/types/sockaddr_in.hh"


namespace poseven
{
	
	inline Nucleus::Owned< fd_t > accept( fd_t listener, sockaddr_in& address )
	{
		return accept< af_inet >( listener, address );
	}
	
	inline void bind( fd_t fd, in_addr_t addr, in_port_t port )
	{
		bind< af_inet >( fd, sockaddr_traits< af_inet >::make( addr, port ) );
	}
	
	inline Nucleus::Owned< fd_t > bind( in_addr_t    addr,
	                                    in_port_t    port,
	                                    socket_type  type = sock_stream )
	{
		Nucleus::Owned< fd_t > fd = socket( pf_inet, type );
		
		bind( fd, addr, port );
		
		return fd;
	}
	
	inline void connect( fd_t fd, in_addr_t addr, in_port_t port )
	{
		connect< af_inet >( fd, sockaddr_traits< af_inet >::make( addr, port ) );
	}
	
	inline Nucleus::Owned< fd_t > connect( in_addr_t    addr,
	                                       in_port_t    port,
	                                       socket_type  type = sock_stream )
	{
		Nucleus::Owned< fd_t > fd = socket( pf_inet, type );
		
		connect( fd, addr, port );
		
		return fd;
	}
	
}

#endif

