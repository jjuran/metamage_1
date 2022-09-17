/*	===========
 *	Resolver.cc
 *	===========
 */

// Standard C/C++
#include <cstdio>

// POSIX
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>


char* inet_ntoa( struct in_addr addr )
{
	static char result[ INET_ADDRSTRLEN ] = { 0 };
	
	std::sprintf( result, "%u.%u.%u.%u", (addr.s_addr >> 24) & 0xFF,
	                                     (addr.s_addr >> 16) & 0xFF,
	                                     (addr.s_addr >>  8) & 0xFF,
	                                     (addr.s_addr      ) & 0xFF );
	
	return result;
}

const char* hstrerror( int errnum )
{
	return "hstrerror() is unimplemented";
}

extern "C" { int h_errno; }
