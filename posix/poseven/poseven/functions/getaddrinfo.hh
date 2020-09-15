/*
	getaddrinfo.hh
	--------------
*/

#ifndef POSEVEN_FUNCTIONS_GETADDRINFO_HH
#define POSEVEN_FUNCTIONS_GETADDRINFO_HH

// poseven
#ifndef POSEVEN_FUNCTIONS_FREEADDRINFO_HH
#include "poseven/functions/freeaddrinfo.hh"
#endif


namespace poseven
{
	
	nucleus::owned< ::addrinfo* > getaddrinfo( const char*        hostname,
	                                           const char*        service,
	                                           const ::addrinfo*  hints = 0 );
	
}

#endif
