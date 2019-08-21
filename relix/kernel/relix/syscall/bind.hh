/*
	bind.hh
	-------
*/

#ifndef RELIX_SYSCALL_BIND_HH
#define RELIX_SYSCALL_BIND_HH

// POSIX
#ifdef __APPLE__
// This is needed to compile with Mac OS X 10.3's headers.
#include <sys/types.h>
#endif
#include <sys/socket.h>


namespace relix
{
	
	int bind( int fd, const struct sockaddr* name, socklen_t namelen );
	
}

#endif
