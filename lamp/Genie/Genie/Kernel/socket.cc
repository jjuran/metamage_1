/*	=========
 *	socket.cc
 *	=========
 */

// POSIX
#include <sys/socket.h>

// relix-kernel
#include "relix/api/assign_fd.hh"
#include "relix/api/first_free_fd.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/IO/OTSocket.hh"
#include "Genie/IO/PairedSocket.hh"


#ifndef SOCK_CLOEXEC
#define SOCK_CLOEXEC  0
#endif

#ifndef SOCK_NONBLOCK
#define SOCK_NONBLOCK  0
#endif


#ifndef __MACOS__
namespace
{
#endif


int socketpair( int domain, int type, int protocol, int fds[2] )
{
	using namespace Genie;
	
	try
	{
		const bool close_on_exec = type & SOCK_CLOEXEC;
		const bool nonblocking   = type & SOCK_NONBLOCK;
		
		boost::intrusive_ptr< vfs::stream > east( new vfs::stream );
		boost::intrusive_ptr< vfs::stream > west( new vfs::stream );
		
		IOPtr san_jose = NewPairedSocket( west, east, nonblocking );
		IOPtr new_york = NewPairedSocket( east, west, nonblocking );
		
		int a = relix::first_free_fd( 3 );
		int b = relix::first_free_fd( a + 1 );
		
		relix::assign_fd( a, *san_jose, close_on_exec );
		relix::assign_fd( b, *new_york, close_on_exec );
		
		fds[ 0 ] = a;
		fds[ 1 ] = b;
	}
	catch ( ... )
	{
		return set_errno_from_exception();
	}
	
	return 0;
}


int socket( int domain, int type, int protocol )
{
	using namespace Genie;
	
	int fd = relix::first_free_fd();
	
	// Assume domain is PF_INET, type is SOCK_STREAM, and protocol is INET_TCP
	
	try
	{
		const bool close_on_exec = type & SOCK_CLOEXEC;
		const bool nonblocking   = type & SOCK_NONBLOCK;
		
		relix::assign_fd( fd,
		                  *New_OT_Socket( nonblocking ),
		                  close_on_exec );
	}
	catch ( ... )
	{
		return set_errno_from_exception();
	}
	
	return fd;
}

#ifndef __MACOS__
}
#endif

