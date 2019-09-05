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

// poseven
#ifndef POSEVEN_FUNCTIONS_CLOSE_HH
#include "poseven/functions/close.hh"
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
	
	nucleus::owned< fd_t > accept( fd_t        listener,
	                               sockaddr*   address,
	                               socklen_t*  length );
	
	inline nucleus::owned< fd_t > accept( fd_t listener )
	{
		return accept( listener, NULL, NULL );
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
		
		return accept( listener, (sockaddr*) &client_address, &length );
	}
	
}

#endif
