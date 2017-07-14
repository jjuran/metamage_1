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


namespace poseven
{
	
	plus::string inet_ntop( const ::in_addr&   addr );
	plus::string inet_ntop( const ::in6_addr&  addr );
	
}

#endif
