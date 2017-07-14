/*
	inet_ntop.hh
	------------
*/

#ifndef POSEVEN_FUNCTIONS_INETNTOP_HH
#define POSEVEN_FUNCTIONS_INETNTOP_HH

// plus
#include "plus/string.hh"

// #include <netinet/in.h>
struct in_addr;
struct in6_addr;

// #include <sys/socket.h>
struct sockaddr;
struct sockaddr_storage;


namespace poseven
{
	
	plus::string inet_ntop( const ::in_addr&   addr );
	plus::string inet_ntop( const ::in6_addr&  addr );
	plus::string inet_ntop( const ::sockaddr&  addr );
	
	inline
	plus::string inet_ntop( const ::sockaddr_storage& addr )
	{
		return inet_ntop( (const ::sockaddr&) addr );
	}
	
}

#endif
