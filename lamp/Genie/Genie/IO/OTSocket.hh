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
	
	class OTSocket : public SocketHandle
	{
		private:
			OpenTransportShare itsOpenTransport;
			Nucleus::Owned< Nitrogen::EndpointRef > endpoint;
			SocketAddress socketAddress;
			SocketAddress peerAddress;
			bool isBound;
		
		public:
			OTSocket( bool isBlocking = true );
			
			~OTSocket();
			
			static TypeCode Type()  { return kSocketType; }
			
			TypeCode ActualType() const  { return Type(); }
			
			bool IsBlocking() const  { return Nitrogen::OTIsBlocking( endpoint ); }
			
			unsigned int SysPoll() const;
			
			int SysRead( char* data, std::size_t byteCount );
			
			int SysWrite( const char* data, std::size_t byteCount );
			
			//void IOCtl( unsigned long request, int* argp );
			
			void SetBlocking   ()  { Nitrogen::OTSetBlocking   ( endpoint ); }
			void SetNonBlocking()  { Nitrogen::OTSetNonBlocking( endpoint ); }
			
			void Bind( const sockaddr& local, socklen_t len );
			
			void Listen( int backlog );
			
			std::auto_ptr< IOHandle > Accept( sockaddr& client, socklen_t& len );
			
			void Connect( const sockaddr& server, socklen_t len );
			
			const SocketAddress& GetSockName() const  { return socketAddress; }
			const SocketAddress& GetPeerName() const  { return peerAddress;   }
			
			void ShutdownReading()  {}
			void ShutdownWriting();
	};
	
}

#endif

