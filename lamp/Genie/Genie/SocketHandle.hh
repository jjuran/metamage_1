/*	===============
 *	SocketHandle.hh
 *	===============
 */

#ifndef GENIE_SOCKETHANDLE_HH
#define GENIE_SOCKETHANDLE_HH

// Nitrogen
#include "Nitrogen/OpenTransport.h"

// Nitrogen Extras / Templates
#include "Templates/DataPointer.h"

// Genie
#include "Genie/IORef.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NX = NitrogenExtras;
	
	IORef NewSocket( bool blockingMode = true );
	
	typedef int socklen_t;
	
	typedef const InetAddress* ConstSockAddrParam;
	typedef       InetAddress*      SockAddrParam;
	
	//typedef NX::DataPtr< OTAddress, int > SocketAddress;
	typedef NX::DataPtr< InetAddress, int > SocketAddress;
	
	struct Accept_Result
	{
		IORef          socket;
		SocketAddress  address;
		
		Accept_Result()  {};
		
		Accept_Result( IORef          socket,
		               SocketAddress  address )
		:
			socket ( socket  ),
			address( address )
		{}
	};
	
	class SocketHandle
	{
		private:
			N::Owned< N::EndpointRef > endpoint;
			SocketAddress fSocketAddress;
			SocketAddress fPeerAddress;
			bool fBound;
		
		public:
			SocketHandle( bool blocking = true );
			
			~SocketHandle();
			
			bool IsASocket() const  { return true; }
			
			const SocketAddress& GetSockName() const  { return fSocketAddress; }
			const SocketAddress& GetPeerName() const  { return fPeerAddress;   }
			
			void SetBlocking   ()  { N::OTSetBlocking   ( endpoint ); }
			void SetNonBlocking()  { N::OTSetNonBlocking( endpoint ); }
			
			int Read( char* data, std::size_t byteCount );
			
			int Write( const char* data, std::size_t byteCount );
			
			void Bind( ConstSockAddrParam local, socklen_t len );
			
			void Listen( int backlog );
			
			IORef Accept( SockAddrParam client, socklen_t& len );
			
			Accept_Result Accept();
			
			void Connect( ConstSockAddrParam server, socklen_t len );
	};
	
}

#endif

