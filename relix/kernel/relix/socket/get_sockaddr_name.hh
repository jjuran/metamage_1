/*
	get_sockaddr_name.hh
	--------------------
*/

#ifndef RELIX_SOCKET_GETSOCKADDRNAME_HH
#define RELIX_SOCKET_GETSOCKADDRNAME_HH

// POSIX
#ifdef __APPLE__
// This is needed to compile with Mac OS X 10.3's headers.
#include <sys/types.h>
#endif
#include <sys/socket.h>

#ifdef __APPLE__
#ifndef _SOCKLEN_T
typedef int socklen_t;
#endif
#endif


namespace relix
{
	
	void get_sockaddr_name( const sockaddr& addr, struct sockaddr* name, socklen_t* namelen );
	
}

#endif
