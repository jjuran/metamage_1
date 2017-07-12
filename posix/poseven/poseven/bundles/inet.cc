/*
	inet.cc
	-------
*/

// poseven
#ifndef POSEVEN_BUNDLES_INET_HH
#include "poseven/bundles/inet.hh"
#endif
#ifndef POSEVEN_FUNCTIONS_GETADDRINFO_HH
#include "poseven/functions/getaddrinfo.hh"
#endif


namespace poseven
{
	
	namespace n = nucleus;
	
	n::owned< fd_t > connect( const char* hostname, const char* service )
	{
		addrinfo hints = { 0 };
		
		hints.ai_socktype = SOCK_STREAM;
		
		n::owned< addrinfo* > ai_box = getaddrinfo( hostname, service, &hints );
		
		addrinfo const* const ai = ai_box.get();
		
		const protocol_family pf = protocol_family( ai->ai_family );
		
		n::owned< fd_t > fd = socket( pf, sock_stream );
		
		connect( fd, *ai->ai_addr, ai->ai_addrlen );
		
		return fd;
	}
	
}
