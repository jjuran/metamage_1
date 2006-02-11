/*	===============
 *	SocketHandle.hh
 *	===============
 */

#ifndef GENIE_SOCKETHANDLE_HH
#define GENIE_SOCKETHANDLE_HH

// POSIX
#include <sys/types.h>
#include <sys/socket.h>

// Nitrogen
#include "Nitrogen/OpenTransport.h"

// Nitrogen Extras / Templates
#include "Templates/DataPointer.h"

// Genie
#include "Genie/IORef.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace NX = NitrogenExtras;
	
	IORef NewSocket( bool blockingMode = true );
	
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
	
	class SocketHandle : public IOStream
	{
		private:
			NN::Owned< N::EndpointRef > endpoint;
			SocketAddress fSocketAddress;
			SocketAddress fPeerAddress;
			bool fBound;
		
		public:
			SocketHandle( bool blocking = true );
			
			~SocketHandle();
			
			bool IsASocket() const  { return true; }
			
			const SocketAddress& GetSockName() const  { return fSocketAddress; }
			const SocketAddress& GetPeerName() const  { return fPeerAddress;   }
			
			bool IsBlocking() const  { return N::OTIsBlocking( endpoint ); }
			
			void SetBlocking   ()  { N::OTSetBlocking   ( endpoint ); }
			void SetNonBlocking()  { N::OTSetNonBlocking( endpoint ); }
			
			unsigned int SysPoll() const;
			
			int SysRead( char* data, std::size_t byteCount );
			
			int SysWrite( const char* data, std::size_t byteCount );
			
			void Bind( ConstSockAddrParam local, socklen_t len );
			
			void Listen( int backlog );
			
			IORef Accept( SockAddrParam client, socklen_t& len );
			
			Accept_Result Accept();
			
			void Connect( ConstSockAddrParam server, socklen_t len );
	};
	
}

#endif

