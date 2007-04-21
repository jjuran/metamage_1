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
	
	
	struct OTInitialization
	{
		OTInitialization()   { N::InitOpenTransport();  }
		~OTInitialization()  { N::CloseOpenTransport(); }
	};
	
	static void Check_InitOpenTransport()
	{
		static OTInitialization init;
	}
	
	// Special guest appearance
	static InetSvcRef InternetServices()
	{
		Check_InitOpenTransport();
		
		static NN::Owned< InetSvcRef >
		       gInetSvcRef = N::OTOpenInternetServices( kDefaultInternetServicesPath );
		
		return gInetSvcRef;
	}
	
	REGISTER_SYSTEM_CALL( InternetServices );
	
	
	static int socket( int /*domain*/, int /*type*/, int /*protocol*/ )
	{
		Check_InitOpenTransport();
		
		int fd = LowestUnusedFileDescriptor();
		
		// Assume domain is PF_INET, type is SOCK_STREAM, and protocol is INET_TCP
		
		try
		{
			AssignFileDescriptor( fd, boost::shared_ptr< IOHandle >( new SocketHandle ) );
		}
		catch ( ... )
		{
			return GetErrnoFromExceptionInSystemCall();
		}
		
		return fd;
	}
	
	REGISTER_SYSTEM_CALL( socket );
	
	static int bind( int fd, const struct sockaddr* name, socklen_t namelen )
	{
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
			return GetErrnoFromExceptionInSystemCall();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( bind );
	
	static int listen( int fd, int backlog )
	{
		try
		{
			SocketHandle& sock = GetFileHandleWithCast< SocketHandle >( fd );
			
			sock.Listen( backlog );
		}
		catch ( ... )
		{
			return GetErrnoFromExceptionInSystemCall();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( listen );
	
	static int accept( int listener, struct sockaddr *addr, socklen_t *addrlen )
	{
		try
		{
			SocketHandle& sock = GetFileHandleWithCast< SocketHandle >( listener );
			
			std::auto_ptr< SocketHandle > incoming( sock.Accept( (InetAddress*) addr, *addrlen ) );
			
			int fd = LowestUnusedFileDescriptor();
			
			AssignFileDescriptor( fd, boost::shared_ptr< IOHandle >( incoming ) );
			
			return fd;
		}
		catch ( ... )
		{
			return GetErrnoFromExceptionInSystemCall();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( accept );
	
	static int connect( int fd, const struct sockaddr* serv_addr, socklen_t addrlen )
	{
		// Assume sin_family is AF_INET
		
		const InetAddress* inetAddress = reinterpret_cast< const InetAddress* >( serv_addr );
		
		try
		{
			SocketHandle& sock = GetFileHandleWithCast< SocketHandle >( fd );
			
			sock.Connect( inetAddress, addrlen );
		}
		catch ( ... )
		{
			return GetErrnoFromExceptionInSystemCall();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( connect );
	
	static int getsockname( int fd, struct sockaddr* name, socklen_t* namelen )
	{
		try
		{
			SocketHandle& sock = GetFileHandleWithCast< SocketHandle >( fd );
			
			const SocketAddress& address = sock.GetSockName();
			
			*namelen = address.Len();
			
			std::memcpy( name, address.Get(), address.Len() );
		}
		catch ( ... )
		{
			return GetErrnoFromExceptionInSystemCall();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( getsockname );
	
	static int getpeername( int fd, struct sockaddr* name, socklen_t* namelen )
	{
		try
		{
			SocketHandle& sock = GetFileHandleWithCast< SocketHandle >( fd );
			
			const SocketAddress& address = sock.GetPeerName();
			
			*namelen = address.Len();
			
			std::memcpy( name, address.Get(), address.Len() );
		}
		catch ( ... )
		{
			return GetErrnoFromExceptionInSystemCall();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( getpeername );
	
	static int shutdown( int fd, int how )
	{
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
			return GetErrnoFromExceptionInSystemCall();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( shutdown );
	
}

