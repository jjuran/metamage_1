/*
	getaddrinfo.cc
	--------------
*/

#include "poseven/functions/getaddrinfo.hh"

// poseven
#include "poseven/types/gai_error.hh"


namespace poseven
{
	
	nucleus::owned< ::addrinfo* > getaddrinfo( const char*        hostname,
	                                           const char*        service,
	                                           const ::addrinfo*  hints )
	{
		addrinfo* result;
		
		throw_gai_error( ::getaddrinfo( hostname, service, hints, &result ) );
		
		return nucleus::owned< ::addrinfo* >::seize( result );
	}
	
}
