/*	=========
 *	socket.cc
 *	=========
 */

// Standard C/C++
#include <cstring>

// POSIX
#include "sys/socket.h"

// Nitrogen
#include "Nitrogen/OpenTransportProviders.h"

// Kerosene/Common
#include "SystemCalls.hh"

// Genie
#include "Genie/FileDescriptors.hh"
#include "Genie/IO/SocketStream.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


namespace Genie
{
	
	DECLARE_MODULE_INIT( Kernel_socket )
	DEFINE_MODULE_INIT(  Kernel_socket )
	
	namespace NN = Nucleus;
	
	
	static int socket( int /*domain*/, int /*type*/, int /*protocol*/ )
	{
		SystemCallFrame frame( "socket" );
		
		int fd = LowestUnusedFileDescriptor();
		
		// Assume domain is PF_INET, type is SOCK_STREAM, and protocol is INET_TCP
		
		try
		{
			AssignFileDescriptor( fd, boost::shared_ptr< IOHandle >( new SocketHandle ) );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return fd;
	}
	
	REGISTER_SYSTEM_CALL( socket );
	
	static int bind( int fd, const struct sockaddr* name, socklen_t namelen )
	{
		SystemCallFrame frame( "bind" );
		
		const InetAddress* inetAddress = reinterpret_cast< const InetAddress* >( name );
		
		try
		{
			SocketHandle& sock = GetFileHandleWithCast< SocketHandle >( fd );
			
			sock.Bind( inetAddress, namelen );
			
			/*
			if ( files[ sockfd ].handle.IsType( kSocketDescriptor ) )
			{
				SocketHandle& sock = IORef_Cast< SocketHandle >( files[ sockfd ].handle );
				sock.Bind( inetAddress, namelen );
			}
			else
			{
				return CurrentProcess().SetErrno( ENOTSOCK );
			}
			*/
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( bind );
	
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
	
	REGISTER_SYSTEM_CALL( listen );
	
	static int accept( int listener, struct sockaddr *addr, socklen_t *addrlen )
	{
		SystemCallFrame frame( "accept" );
		
		try
		{
			SocketHandle& sock = GetFileHandleWithCast< SocketHandle >( listener );
			
			std::auto_ptr< IOHandle > incoming( sock.Accept( (InetAddress*) addr, *addrlen ) );
			
			int fd = LowestUnusedFileDescriptor();
			
			AssignFileDescriptor( fd, boost::shared_ptr< IOHandle >( incoming ) );
			
			return fd;
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( accept );
	
	static int connect( int fd, const struct sockaddr* serv_addr, socklen_t addrlen )
	{
		SystemCallFrame frame( "connect" );
		
		// Assume sin_family is AF_INET
		
		const InetAddress* inetAddress = reinterpret_cast< const InetAddress* >( serv_addr );
		
		try
		{
			SocketHandle& sock = GetFileHandleWithCast< SocketHandle >( fd );
			
			sock.Connect( inetAddress, addrlen );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( connect );
	
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
	
	REGISTER_SYSTEM_CALL( getsockname );
	
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
	
	REGISTER_SYSTEM_CALL( getpeername );
	
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
	
	REGISTER_SYSTEM_CALL( shutdown );
	
}

