/*	===========
 *	OTSocket.hh
 *	===========
 */

#ifndef GENIE_IO_OTSOCKET_HH
#define GENIE_IO_OTSOCKET_HH

// Boost
#include <boost/shared_ptr.hpp>

// POSIX
#include <sys/socket.h>

// Nitrogen
#include "Nitrogen/OpenTransport.h"
#include "Nitrogen/OpenTransportProviders.h"

// Genie
#include "Genie/IO/SocketStream.hh"
#include "Genie/Utilities/ShareOpenTransport.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	/*
	typedef const InetAddress* ConstSockAddrParam;
	typedef       InetAddress*      SockAddrParam;
	*/
	
	struct SocketAddress
	{
		InetAddress  address;
		socklen_t    length;
		
		SocketAddress()  {}
		SocketAddress( const InetAddress* addr, socklen_t len ) : address( *addr ), length( len )  {}
		
		InetAddress const* Get() const  { return &address; }
		InetAddress      * Get()        { return &address; }
		
		socklen_t Len() const  { return length; }
		
		void Assign( const InetAddress* addr, socklen_t len )
		{
			address = *addr;
			length  = len;
		}
	};
	
	class OTSocket : public SocketHandle
	{
		private:
			OpenTransportShare itsOpenTransport;
			NN::Owned< N::EndpointRef > endpoint;
			SocketAddress socketAddress;
			SocketAddress peerAddress;
			bool isBound;
		
		public:
			OTSocket( bool isBlocking = true );
			
			~OTSocket();
			
			static TypeCode Type()  { return kSocketType; }
			
			TypeCode ActualType() const  { return Type(); }
			
			bool IsBlocking() const  { return N::OTIsBlocking( endpoint ); }
			
			unsigned int SysPoll() const;
			
			int SysRead( char* data, std::size_t byteCount );
			
			int SysWrite( const char* data, std::size_t byteCount );
			
			//void IOCtl( unsigned long request, int* argp );
			
			void SetBlocking   ()  { N::OTSetBlocking   ( endpoint ); }
			void SetNonBlocking()  { N::OTSetNonBlocking( endpoint ); }
			
			void Bind( ConstSockAddrParam local, socklen_t len );
			
			void Listen( int backlog );
			
			std::auto_ptr< IOHandle > Accept( SockAddrParam client, socklen_t& len );
			
			void Connect( ConstSockAddrParam server, socklen_t len );
			
			const SocketAddress& GetSockName() const  { return socketAddress; }
			const SocketAddress& GetPeerName() const  { return peerAddress;   }
			
			void ShutdownReading()  {}
			void ShutdownWriting();
	};
	
}

#endif

