/*	=========
 *	socket.cc
 *	=========
 */

// Standard C/C++
#include <cstring>

// Mac OS
#ifndef __OPENTRANSPORTPROVIDERS__
#include <OpenTransportProviders.h>
#endif

// POSIX
#include "sys/socket.h"

// Genie
#include "Genie/FileDescriptors.hh"
#include "Genie/IO/Conduit.hh"
#include "Genie/IO/OTSocket.hh"
#include "Genie/IO/PairedSocket.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


#ifndef SOCK_CLOEXEC
#define SOCK_CLOEXEC  0
#endif

#ifndef SOCK_NONBLOCK
#define SOCK_NONBLOCK  0
#endif


namespace Genie
{
	
	static int socketpair( int domain, int type, int protocol, int fds[2] )
	{
		SystemCallFrame frame( "socketpair" );
		
		try
		{
			const bool close_on_exec = type & SOCK_CLOEXEC;
			const bool nonblocking   = type & SOCK_NONBLOCK;
			
			boost::shared_ptr< Conduit > east( new Conduit );
			boost::shared_ptr< Conduit > west( new Conduit );
			
			boost::shared_ptr< IOHandle > san_jose = NewPairedSocket( west, east, nonblocking );
			boost::shared_ptr< IOHandle > new_york = NewPairedSocket( east, west, nonblocking );
			
			int a = LowestUnusedFileDescriptor( 3 );
			int b = LowestUnusedFileDescriptor( a + 1 );
			
			AssignFileDescriptor( a, san_jose, close_on_exec );
			AssignFileDescriptor( b, new_york, close_on_exec );
			
			fds[ 0 ] = a;
			fds[ 1 ] = b;
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	
	static int socket( int domain, int type, int protocol )
	{
		SystemCallFrame frame( "socket" );
		
		int fd = LowestUnusedFileDescriptor();
		
		// Assume domain is PF_INET, type is SOCK_STREAM, and protocol is INET_TCP
		
		try
		{
			const bool close_on_exec = type & SOCK_CLOEXEC;
			const bool nonblocking   = type & SOCK_NONBLOCK;
			
			AssignFileDescriptor( fd,
			                      seize_ptr( new OTSocket( nonblocking ) ),
			                      close_on_exec );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return fd;
	}
	
	
	static int bind( int fd, const struct sockaddr* name, socklen_t namelen )
	{
		SystemCallFrame frame( "bind" );
		
		try
		{
			SocketHandle& sock = GetFileHandleWithCast< SocketHandle >( fd );
			
			sock.Bind( *name, namelen );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	
	static int listen( int fd, int backlog )
	{
		SystemCallFrame frame( "listen" );
		
		try
		{
			SocketHandle& sock = GetFileHandleWithCast< SocketHandle >( fd );
			
			sock.Listen( backlog );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	
	static int accept( int listener, struct sockaddr *addr, socklen_t *addrlen )
	{
		SystemCallFrame frame( "accept" );
		
		try
		{
			SocketHandle& sock = GetFileHandleWithCast< SocketHandle >( listener );
			
			if ( addrlen == NULL  &&  addr != NULL )
			{
				// If you pass the address buffer you must indicate the size
				frame.SetErrno( EINVAL );
			}
			
			sockaddr dummy_addr;
			
			socklen_t dummy_length = sizeof dummy_addr;
			
			sockaddr& address = addr != NULL ? *addr : dummy_addr;
			
			// addr != NULL  implies  addrlen != NULL
			socklen_t& length = addr != NULL ? *addrlen : dummy_length;
			
			std::auto_ptr< IOHandle > incoming( sock.Accept( address, length ) );
			
			int fd = LowestUnusedFileDescriptor();
			
			AssignFileDescriptor( fd, boost::shared_ptr< IOHandle >( incoming ) );
			
			if ( addr == NULL  &&  addrlen != NULL )
			{
				// You can pass a NULL address buffer and still get the size back
				*addrlen = length;
			}
			
			return fd;
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	
	static int connect( int fd, const struct sockaddr* serv_addr, socklen_t addrlen )
	{
		SystemCallFrame frame( "connect" );
		
		// Assume sin_family is AF_INET
		
		try
		{
			SocketHandle& sock = GetFileHandleWithCast< SocketHandle >( fd );
			
			sock.Connect( *serv_addr, addrlen );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	
	static int getsockname( int fd, struct sockaddr* name, socklen_t* namelen )
	{
		SystemCallFrame frame( "getsockname" );
		
		try
		{
			SocketHandle& sock = GetFileHandleWithCast< SocketHandle >( fd );
			
			const SocketAddress& address = sock.GetSockName();
			
			*namelen = address.Len();
			
			std::memcpy( name, address.Get(), address.Len() );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	
	static int getpeername( int fd, struct sockaddr* name, socklen_t* namelen )
	{
		SystemCallFrame frame( "getpeername" );
		
		try
		{
			SocketHandle& sock = GetFileHandleWithCast< SocketHandle >( fd );
			
			const SocketAddress& address = sock.GetPeerName();
			
			*namelen = address.Len();
			
			std::memcpy( name, address.Get(), address.Len() );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	
	static int shutdown( int fd, int how )
	{
		SystemCallFrame frame( "shutdown" );
		
		try
		{
			SocketHandle& sock = GetFileHandleWithCast< SocketHandle >( fd );
			
			int flags = how + 1;
			
			bool stop_reading = flags & 1;
			bool stop_writing = flags & 2;
			
			if ( stop_reading )
			{
				sock.ShutdownReading();
			}
			
			if ( stop_writing )
			{
				sock.ShutdownWriting();
			}
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( socketpair  );
	REGISTER_SYSTEM_CALL( socket      );
	REGISTER_SYSTEM_CALL( bind        );
	REGISTER_SYSTEM_CALL( listen      );
	REGISTER_SYSTEM_CALL( accept      );
	REGISTER_SYSTEM_CALL( connect     );
	REGISTER_SYSTEM_CALL( getsockname );
	REGISTER_SYSTEM_CALL( getpeername );
	REGISTER_SYSTEM_CALL( shutdown    );
	
	#pragma force_active reset
	
}

