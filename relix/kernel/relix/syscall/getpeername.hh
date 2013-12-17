/*
	getpeername.hh
	--------------
*/

#ifndef RELIX_SYSCALL_GETPEERNAME_HH
#define RELIX_SYSCALL_GETPEERNAME_HH

// POSIX
#include <sys/socket.h>


namespace relix
{
	
	int getpeername( int fd, struct sockaddr* name, socklen_t* namelen );
	
}

#endif

