/*	===========
 *	Resolver.cc
 *	===========
 */

// POSIX
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>


char* inet_ntoa( struct in_addr addr )
{
	static char result[ INET_ADDRSTRLEN ] = { 0 };
	
	return (char*) inet_ntop( AF_INET, &addr, result, sizeof result );
}

const char* hstrerror( int errnum )
{
	return "hstrerror() is unimplemented";
}

extern "C" { int h_errno; }
