/*
	bind.hh
	-------
*/

#ifndef RELIX_SYSCALL_BIND_HH
#define RELIX_SYSCALL_BIND_HH

// POSIX
#include <sys/socket.h>


namespace relix
{
	
	int bind( int fd, const struct sockaddr* name, socklen_t namelen );
	
}

#endif

