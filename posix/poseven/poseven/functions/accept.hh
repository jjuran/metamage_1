// accept.hh
// ---------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_ACCEPT_HH
#define POSEVEN_FUNCTIONS_ACCEPT_HH

// POSIX
#include <unistd.h>

// poseven
#include "poseven/functions/close.hh"
#include "poseven/types/errno_t.hh"
#include "poseven/types/fd_t.hh"
#include "poseven/types/sockaddr.hh"


namespace poseven
{
	
	namespace detail
	{
		
		inline nucleus::owned< fd_t > accept( fd_t        listener,
		                                      sockaddr*   address,
		                                      socklen_t*  length )
		{
			int result = throw_posix_result( ::accept( listener,
			                                           address,
			                                           length ) );
			
			return nucleus::owned< fd_t >::seize( fd_t( result ) );
		}
		
	}
	
	inline nucleus::owned< fd_t > accept( fd_t listener )
	{
		return detail::accept( listener, NULL, NULL );
	}
	
	template < address_family af >
	inline
	nucleus::owned< fd_t >
	//
	accept( fd_t                                           listener,
	        typename sockaddr_traits< af >::address_type&  client_address )
	{
		typedef typename sockaddr_traits< af >::address_type address_type;
		
		socklen_t length = sizeof (address_type);
		
		return detail::accept( listener, (sockaddr*) &client_address, &length );
	}
	
}

#endif

