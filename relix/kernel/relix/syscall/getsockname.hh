/*
	getsockname.hh
	--------------
*/

#ifndef RELIX_SYSCALL_GETSOCKNAME_HH
#define RELIX_SYSCALL_GETSOCKNAME_HH

// POSIX
#include <sys/socket.h>


namespace relix
{
	
	int getsockname( int fd, struct sockaddr* name, socklen_t* namelen );
	
}

#endif

