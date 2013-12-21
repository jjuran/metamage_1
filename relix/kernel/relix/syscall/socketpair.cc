/*
	socketpair.cc
	-------------
*/

#include "relix/syscall/socketpair.hh"

// POSIX
#include <sys/socket.h>

// vfs
#include "vfs/filehandle.hh"

// relix-kernel
#include "relix/api/assign_fd.hh"
#include "relix/api/errno.hh"
#include "relix/api/first_free_fd.hh"
#include "relix/api/new_paired_socket.hh"


#ifndef SOCK_CLOEXEC
#define SOCK_CLOEXEC  0
#endif

#ifndef SOCK_NONBLOCK
#define SOCK_NONBLOCK  0
#endif


namespace relix
{
	
	int socketpair( int domain, int type, int protocol, int fds[2] )
	{
		try
		{
			const bool close_on_exec = type & SOCK_CLOEXEC;
			const bool nonblocking   = type & SOCK_NONBLOCK;
			
			boost::intrusive_ptr< vfs::stream > east( new vfs::stream );
			boost::intrusive_ptr< vfs::stream > west( new vfs::stream );
			
			vfs::filehandle_ptr san_jose = new_paired_socket( west, east, nonblocking );
			vfs::filehandle_ptr new_york = new_paired_socket( east, west, nonblocking );
			
			int a = first_free_fd( 3 );
			int b = first_free_fd( a + 1 );
			
			assign_fd( a, *san_jose, close_on_exec );
			assign_fd( b, *new_york, close_on_exec );
			
			fds[ 0 ] = a;
			fds[ 1 ] = b;
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
}

