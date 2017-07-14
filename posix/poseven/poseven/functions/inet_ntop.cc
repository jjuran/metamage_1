/*
	inet_ntop.cc
	------------
*/

#include "poseven/functions/inet_ntop.hh"

// POSIX
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	plus::string inet_ntop( const ::in_addr& addr )
	{
		char buffer[ INET_ADDRSTRLEN ];
		
		const char* p = ::inet_ntop( AF_INET, &addr, buffer, sizeof buffer );
		
		if ( p == NULL )
		{
			throw_errno( errno );
		}
		
		return p;
	}
	
	plus::string inet_ntop( const ::in6_addr& addr )
	{
		char buffer[ INET6_ADDRSTRLEN ];
		
		const char* p = ::inet_ntop( AF_INET6, &addr, buffer, sizeof buffer );
		
		if ( p == NULL )
		{
			throw_errno( errno );
		}
		
		return p;
	}
	
}
