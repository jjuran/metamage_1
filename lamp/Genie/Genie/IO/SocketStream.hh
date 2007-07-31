/*	===============
 *	SocketStream.hh
 *	===============
 */

#ifndef GENIE_IO_SOCKET_HH
#define GENIE_IO_SOCKET_HH

// Boost
#include <boost/shared_ptr.hpp>

// POSIX
#include <sys/socket.h>

// Nitrogen
#include "Nitrogen/OpenTransport.h"
#include "Nitrogen/OpenTransportProviders.h"

// Nitrogen Extras / Templates
//#include "Templates/DataPointer.h"

// Genie
#include "Genie/IO/Stream.hh"
#include "Genie/Utilities/ShareOpenTransport.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	//namespace NX = NitrogenExtras;
	
	typedef const InetAddress* ConstSockAddrParam;
	typedef       InetAddress*      SockAddrParam;
	
	//typedef NX::DataPtr< InetAddress, int > SocketAddress;
	
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
	
	class SocketHandle;
	
	struct Accept_Result
	{
		boost::shared_ptr< SocketHandle >  socket;
		SocketAddress                      address;
		
		Accept_Result()  {};
		
		Accept_Result( boost::shared_ptr< SocketHandle >  socket,
		               SocketAddress                      address )
		:
			socket ( socket  ),
			address( address )
		{}
	};
	
	class SocketHandle : public StreamHandle
	{
		private:
			OpenTransportShare itsOpenTransport;
			NN::Owned< N::EndpointRef > endpoint;
			SocketAddress socketAddress;
			SocketAddress peerAddress;
			bool isBound;
		
		public:
			SocketHandle( bool isBlocking = true );
			
			virtual ~SocketHandle();
			
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
			
			std::auto_ptr< SocketHandle > Accept( SockAddrParam client, socklen_t& len );
			
			Accept_Result Accept();
			
			void Connect( ConstSockAddrParam server, socklen_t len );
			
			const SocketAddress& GetSockName() const  { return socketAddress; }
			const SocketAddress& GetPeerName() const  { return peerAddress;   }
			
			void ShutdownReading()  {}
			void ShutdownWriting();
	};
	
}

#endif

