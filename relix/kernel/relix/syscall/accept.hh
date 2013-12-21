/*
	accept.hh
	---------
*/

#ifndef RELIX_SYSCALL_ACCEPT_HH
#define RELIX_SYSCALL_ACCEPT_HH

// POSIX
#include <sys/socket.h>


namespace relix
{
	
	int accept( int listener, struct sockaddr* addr, socklen_t* addrlen );
	
}

#endif

