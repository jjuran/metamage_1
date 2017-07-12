/*
	inet.hh
	-------
*/

#ifndef POSEVEN_BUNDLES_INET_HH
#define POSEVEN_BUNDLES_INET_HH

// iota
#include "iota/string_traits.hh"

// poseven
#ifndef POSEVEN_FUNCTIONS_ACCEPT_HH
#include "poseven/functions/accept.hh"
#endif
#ifndef POSEVEN_FUNCTIONS_BIND_HH
#include "poseven/functions/bind.hh"
#endif
#ifndef POSEVEN_FUNCTIONS_CONNECT_HH
#include "poseven/functions/connect.hh"
#endif
#ifndef POSEVEN_FUNCTIONS_SOCKET_HH
#include "poseven/functions/socket.hh"
#endif
#ifndef POSEVEN_TYPES_SOCKADDRIN_HH
#include "poseven/types/sockaddr_in.hh"
#endif


namespace poseven
{
	
	inline
	nucleus::owned< fd_t > accept( fd_t listener, sockaddr_in& address )
	{
		return accept< af_inet >( listener, address );
	}
	
	inline
	void bind( fd_t fd, in_addr_t addr, in_port_t port )
	{
		bind< af_inet >( fd, sockaddr_traits< af_inet >::make( addr, port ) );
	}
	
	inline
	nucleus::owned< fd_t > bind( in_addr_t    addr,
	                             in_port_t    port,
	                             socket_type  type = sock_stream )
	{
		nucleus::owned< fd_t > fd = socket( pf_inet, type );
		
		bind( fd, addr, port );
		
		return fd;
	}
	
	inline
	void connect( fd_t fd, in_addr_t addr, in_port_t port )
	{
		connect< af_inet >( fd,
		                    sockaddr_traits< af_inet >::make( addr, port ) );
	}
	
	inline
	nucleus::owned< fd_t > connect( in_addr_t    addr,
	                                in_port_t    port,
	                                socket_type  type = sock_stream )
	{
		nucleus::owned< fd_t > fd = socket( pf_inet, type );
		
		connect( fd, addr, port );
		
		return fd;
	}
	
	nucleus::owned< fd_t > connect( const char* hostname, const char* service );
	
	template < class Str1, class Str2 >
	inline
	nucleus::owned< fd_t > connect( const Str1& hostname, const Str2& service )
	{
		using iota::get_string_c_str;
		
		return connect( get_string_c_str( hostname ),
		                get_string_c_str( service  ) );
	}
	
}

#endif
