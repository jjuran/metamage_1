/*	=========
 *	socket.cc
 *	=========
 */

// POSIX
#include <sys/socket.h>

// Standard C
#include <errno.h>

// Standard C/C++
#include <cstring>

// plus
#include "plus/conduit.hh"

// vfs
#include "vfs/filehandle/primitives/accept.hh"
#include "vfs/filehandle/primitives/bind.hh"
#include "vfs/filehandle/primitives/connect.hh"
#include "vfs/filehandle/primitives/getpeername.hh"
#include "vfs/filehandle/primitives/getsockname.hh"
#include "vfs/filehandle/primitives/listen.hh"
#include "vfs/filehandle/primitives/shutdown.hh"

// relix-kernel
#include "relix/api/assign_fd.hh"
#include "relix/api/first_free_fd.hh"
#include "relix/api/get_fd_handle.hh"

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

using plus::conduit;


int socketpair( int domain, int type, int protocol, int fds[2] )
{
	using namespace Genie;
	
	try
	{
		const bool close_on_exec = type & SOCK_CLOEXEC;
		const bool nonblocking   = type & SOCK_NONBLOCK;
		
		boost::intrusive_ptr< conduit > east( new conduit );
		boost::intrusive_ptr< conduit > west( new conduit );
		
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


int bind( int fd, const struct sockaddr* name, socklen_t namelen )
{
	using namespace Genie;
	
	try
	{
		bind( relix::get_fd_handle( fd ), *name, namelen );
	}
	catch ( ... )
	{
		return set_errno_from_exception();
	}
	
	return 0;
}


int listen( int fd, int backlog )
{
	using namespace Genie;
	
	try
	{
		listen( relix::get_fd_handle( fd ), backlog );
	}
	catch ( ... )
	{
		return set_errno_from_exception();
	}
	
	return 0;
}


int accept( int listener, struct sockaddr *addr, socklen_t *addrlen )
{
	using namespace Genie;
	
	try
	{
		if ( addrlen == NULL  &&  addr != NULL )
		{
			// If you pass the address buffer you must indicate the size
			set_errno( EINVAL );
		}
		
		sockaddr dummy_addr;
		
		socklen_t dummy_length = sizeof dummy_addr;
		
		sockaddr& address = addr != NULL ? *addr : dummy_addr;
		
		// addr != NULL  implies  addrlen != NULL
		socklen_t& length = addr != NULL ? *addrlen : dummy_length;
		
		vfs::filehandle_ptr incoming = accept( relix::get_fd_handle( listener ), address, length );
		
		int fd = relix::first_free_fd();
		
		relix::assign_fd( fd, *incoming );
		
		if ( addr == NULL  &&  addrlen != NULL )
		{
			// You can pass a NULL address buffer and still get the size back
			*addrlen = length;
		}
		
		return fd;
	}
	catch ( ... )
	{
		return set_errno_from_exception();
	}
	
	return 0;
}


int connect( int fd, const struct sockaddr* serv_addr, socklen_t addrlen )
{
	using namespace Genie;
	
	// Assume sin_family is AF_INET
	
	try
	{
		connect( relix::get_fd_handle( fd ), *serv_addr, addrlen );
	}
	catch ( ... )
	{
		return set_errno_from_exception();
	}
	
	return 0;
}


static inline socklen_t sizeof_sockaddr( const sockaddr& addr )
{
	return sizeof (sockaddr);
}

static void get_sockaddr_name( const sockaddr& addr, struct sockaddr* name, socklen_t* namelen )
{
	const socklen_t size = sizeof_sockaddr( addr );
	
	const size_t n = *namelen < size ? *namelen : size;
	
	std::memcpy( name, &addr, n );
	
	*namelen = size;
}

int getsockname( int fd, struct sockaddr* name, socklen_t* namelen )
{
	using namespace Genie;
	
	try
	{
		const sockaddr& addr = getsockname( relix::get_fd_handle( fd ) );
		
		get_sockaddr_name( addr, name, namelen );
	}
	catch ( ... )
	{
		return set_errno_from_exception();
	}
	
	return 0;
}


int getpeername( int fd, struct sockaddr* name, socklen_t* namelen )
{
	using namespace Genie;
	
	try
	{
		const sockaddr& addr = getpeername( relix::get_fd_handle( fd ) );
		
		get_sockaddr_name( addr, name, namelen );
	}
	catch ( ... )
	{
		return set_errno_from_exception();
	}
	
	return 0;
}


int shutdown( int fd, int how )
{
	using namespace Genie;
	
	try
	{
		shutdown( relix::get_fd_handle( fd ), how );
	}
	catch ( ... )
	{
		return set_errno_from_exception();
	}
	
	return 0;
}

#ifndef __MACOS__
}
#endif

