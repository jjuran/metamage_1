/*	=========
 *	socket.cc
 *	=========
 */

// Standard C
#include <errno.h>

// Standard C/C++
#include <cstring>

// POSIX
#include "sys/socket.h"

// Nitrogen / Carbon
#include "Nitrogen/OpenTransportProviders.h"

// POSeven
//#include "POSeven/Errno.h"

// Io
#include "Io/Exceptions.hh"

// Kerosene/Common
#include "SystemCalls.hh"

// Genie
#include "Genie/Process.hh"
#include "Genie/SocketHandle.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	//namespace P7 = POSeven;
	
	
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
		
		static N::Owned< InetSvcRef >
		       gInetSvcRef = N::OTOpenInternetServices( kDefaultInternetServicesPath );
		
		return gInetSvcRef;
	}
	
	REGISTER_SYSTEM_CALL( InternetServices );
	
	// FIXME:  Duplicate code
	static int LowestUnusedFrom( const FileDescriptorMap& files, int fd )
	{
		while ( files.find( fd ) != files.end() )
		{
			++fd;
		}
		
		return fd;
	}
	
	
	static int socket( int domain, int type, int protocol )
	{
		Check_InitOpenTransport();
		
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		int fd = LowestUnusedFrom( files, 0 );
		
		// Assume domain is PF_INET, type is SOCK_STREAM, and protocol is INET_TCP
		
		try
		{
			files[ fd ] = NewSocket();
		}
		catch ( ... )
		{
			// convert exception
			return -1;
		}
		
		return fd;
	}
	
	REGISTER_SYSTEM_CALL( socket );
	
	static int bind( int sockfd, const struct sockaddr* name, socklen_t namelen )
	{
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		const InetAddress* inetAddress = reinterpret_cast< const InetAddress* >( name );
		
		try
		{
			// FIXME:  Verify socket exists first
			if ( files[ sockfd ].handle.IsType( kSocketDescriptor ) )
			{
				SocketHandle& sock = IORef_Cast< SocketHandle >( files[ sockfd ].handle );
				sock.Bind( inetAddress, namelen );
			}
			else
			{
				return CurrentProcess().SetErrno( ENOTSOCK );
			}
		}
		catch ( ... )
		{
			// Convert the exception
			return -1;
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( bind );
	
	static int listen( int sockfd, int backlog )
	{
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		try
		{
			// FIXME:  Verify socket exists first
			if ( files[ sockfd ].handle.IsType( kSocketDescriptor ) )
			{
				SocketHandle& sock = IORef_Cast< SocketHandle >( files[ sockfd ].handle );
				sock.Listen( backlog );
			}
			else
			{
				return CurrentProcess().SetErrno( ENOTSOCK );
			}
		}
		catch ( ... )
		{
			// Convert the exception
			return -1;
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( listen );
	
	static int accept( int sockfd, struct sockaddr *addr, socklen_t *addrlen )
	{
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		try
		{
			// FIXME:  Verify socket exists first
			if ( files[ sockfd ].handle.IsType( kSocketDescriptor ) )
			{
				SocketHandle& sock = IORef_Cast< SocketHandle >( files[ sockfd ].handle );
				
				//Accept_Result result = sock->Accept();
				
				IORef handle = sock.Accept( (InetAddress*)addr, *addrlen );
				
				int fd = LowestUnusedFrom( files, 0 );
				
				files[ fd ] = FileDescriptor( handle );
				
				return fd;
			}
			else
			{
				return CurrentProcess().SetErrno( ENOTSOCK );
			}
		}
		catch ( Io::NoDataPending& )
		{
			return CurrentProcess().SetErrno( EAGAIN );
		}
		catch ( ... )
		{
			// Convert the exception
			return -1;
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( accept );
	
	static int connect( int sockfd, const struct sockaddr* serv_addr, socklen_t addrlen )
	{
		// Assume sin_family is AF_INET
		
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		const InetAddress* inetAddress = reinterpret_cast< const InetAddress* >( serv_addr );
		
		try
		{
			// FIXME:  Verify socket exists first
			if ( files[ sockfd ].handle.IsType( kSocketDescriptor ) )
			{
				SocketHandle& sock = IORef_Cast< SocketHandle >( files[ sockfd ].handle );
				sock.Connect( inetAddress, addrlen );
			}
			else
			{
				return CurrentProcess().SetErrno( ENOTSOCK );
			}
		}
		catch ( ... )
		{
			// Convert the exception
			return -1;
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( connect );
	
	static int getsockname( int sockfd, struct sockaddr* name, socklen_t* namelen )
	{
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		try
		{
			// FIXME:  Verify socket exists first
			if ( files[ sockfd ].handle.IsType( kSocketDescriptor ) )
			{
				SocketHandle& sock = IORef_Cast< SocketHandle >( files[ sockfd ].handle );
				
				const SocketAddress& address = sock.GetSockName();
				
				*namelen = address.Len();
				std::memcpy( name, address.Get(), address.Len() );
			}
			else
			{
				return CurrentProcess().SetErrno( ENOTSOCK );
			}
		}
		catch ( ... )
		{
			// Convert the exception
			return -1;
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( getsockname );
	
	static int getpeername( int sockfd, struct sockaddr* name, socklen_t* namelen )
	{
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		try
		{
			// FIXME:  Verify socket exists first
			if ( files[ sockfd ].handle.IsType( kSocketDescriptor ) )
			{
				SocketHandle& sock = IORef_Cast< SocketHandle >( files[ sockfd ].handle );
				
				const SocketAddress& address = sock.GetPeerName();
				
				*namelen = address.Len();
				std::memcpy( name, address.Get(), address.Len() );
			}
			else
			{
				return CurrentProcess().SetErrno( ENOTSOCK );
			}
		}
		catch ( ... )
		{
			// Convert the exception
			return -1;
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( getpeername );
	
}

#pragma export on
	
	InetSvcRef InternetServices()
	{
		return Genie::InternetServices();
	}
	
	int socket( int domain, int type, int protocol )
	{
		return Genie::socket( domain, type, protocol );
	}
	
	int bind( int sockfd, const struct sockaddr* name, socklen_t namelen )
	{
		return Genie::bind( sockfd, name, namelen );
	}
	
	int listen( int sockfd, int backlog )
	{
		return Genie::listen( sockfd, backlog );
	}
	
	int accept( int sockfd, struct sockaddr* addr, socklen_t* addrlen )
	{
		return Genie::accept( sockfd, addr, addrlen );
	}
	
	int connect( int sockfd, const struct sockaddr* serv_addr, socklen_t addrlen )
	{
		return Genie::connect( sockfd, serv_addr, addrlen );
	}
	
	int getsockname( int sockfd, struct sockaddr* name, socklen_t* namelen )
	{
		return Genie::getsockname( sockfd, name, namelen );
	}
	
	int getpeername( int sockfd, struct sockaddr* name, socklen_t* namelen )
	{
		return Genie::getpeername( sockfd, name, namelen );
	}
	
#pragma export reset

