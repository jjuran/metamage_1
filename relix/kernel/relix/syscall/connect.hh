/*
	connect.hh
	----------
*/

#ifndef RELIX_SYSCALL_CONNECT_HH
#define RELIX_SYSCALL_CONNECT_HH

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
	
	int connect( int fd, const struct sockaddr* serv_addr, socklen_t addrlen );
	
}

#endif
