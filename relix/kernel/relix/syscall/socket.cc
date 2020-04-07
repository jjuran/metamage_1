/*
	socket.cc
	---------
*/

#include "relix/syscall/socket.hh"

// POSIX
#include <errno.h>
#ifdef __APPLE__
// This is needed to compile with Mac OS X 10.3's headers.
#include <sys/types.h>
#endif
#include <netinet/in.h>
#include <sys/socket.h>

// vfs
#include "vfs/filehandle.hh"

// relix-kernel
#include "relix/api/assign_fd.hh"
#include "relix/api/errno.hh"
#include "relix/api/first_free_fd.hh"
#include "relix/api/new_tcp_socket.hh"
#include "relix/socket/unix_domain_socket.hh"


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
		if ( domain != PF_INET  &&  domain != PF_UNIX )
		{
			return set_errno( EAFNOSUPPORT );
		}
		
		if ( short( type ) != SOCK_STREAM )
		{
			return set_errno( EPROTOTYPE );
		}
		
		try
		{
			const bool close_on_exec = type & SOCK_CLOEXEC;
			const bool nonblocking   = type & SOCK_NONBLOCK;
			
			vfs::filehandle_ptr socket;
			
			switch ( domain )
			{
				case PF_UNIX:
					if ( protocol == 0 )
					{
						socket = new_unix_domain_socket( nonblocking );
					}
					
					break;
				
				case PF_INET:
					if ( protocol == 0  ||  protocol == IPPROTO_TCP )
					{
						socket = new_tcp_socket( nonblocking );
					}
					
					break;
				
				default:
					break;
			}
			
			if ( socket.get() == NULL )
			{
				return set_errno( EPROTONOSUPPORT );
			}
			
			int fd = first_free_fd();
			
			assign_fd( fd, *socket, close_on_exec );
			
			return fd;
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
	}
	
}
