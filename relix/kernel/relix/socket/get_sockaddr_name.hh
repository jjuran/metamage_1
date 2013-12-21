/*
	get_sockaddr_name.hh
	--------------------
*/

#ifndef RELIX_SOCKET_GETSOCKADDRNAME_HH
#define RELIX_SOCKET_GETSOCKADDRNAME_HH

// POSIX
#include <sys/socket.h>


namespace relix
{
	
	void get_sockaddr_name( const sockaddr& addr, struct sockaddr* name, socklen_t* namelen );
	
}

#endif

