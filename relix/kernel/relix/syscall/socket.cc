/*
	socket.cc
	---------
*/

#include "relix/syscall/socket.hh"

// POSIX
#include <sys/socket.h>

// vfs
#include "vfs/filehandle.hh"

// relix-kernel
#include "relix/api/assign_fd.hh"
#include "relix/api/errno.hh"
#include "relix/api/first_free_fd.hh"
#include "relix/api/new_tcp_socket.hh"


#ifndef SOCK_CLOEXEC
#define SOCK_CLOEXEC  0
#endif

#ifndef SOCK_NONBLOCK
#define SOCK_NONBLOCK  0
#endif


namespace relix
{
	
	int socket( int domain, int type, int protocol )
	{
		int fd = first_free_fd();
		
		// Assume domain is PF_INET, type is SOCK_STREAM, and protocol is INET_TCP
		
		try
		{
			const bool close_on_exec = type & SOCK_CLOEXEC;
			const bool nonblocking   = type & SOCK_NONBLOCK;
			
			assign_fd( fd,
						*new_tcp_socket( nonblocking ),
						close_on_exec );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return fd;
	}
	
}

