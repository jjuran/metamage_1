/*
	connect.hh
	----------
*/

#ifndef RELIX_SYSCALL_CONNECT_HH
#define RELIX_SYSCALL_CONNECT_HH

// POSIX
#include <sys/socket.h>


namespace relix
{
	
	int connect( int fd, const struct sockaddr* serv_addr, socklen_t addrlen );
	
}

#endif

