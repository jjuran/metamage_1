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
			OpenTransportShare                       itsOpenTransport;
			Nucleus::Owned< Nitrogen::EndpointRef >  itsEndpoint;
			SocketAddress                            itsSocketAddress;
			SocketAddress                            itsPeerAddress;
			bool                                     itIsBound;
			bool                                     itHasSentFIN;
			bool                                     itHasReceivedFIN;
		
		public:
			OTSocket( bool isBlocking = true );
			
			~OTSocket();
			
			bool IsNonblocking() const  { return !Nitrogen::OTIsBlocking( itsEndpoint ); }
			
			void ReceiveOrderlyDisconnect();
			
			unsigned int SysPoll() const;
			
			int SysRead( char* data, std::size_t byteCount );
			
			int SysWrite( const char* data, std::size_t byteCount );
			
			//void IOCtl( unsigned long request, int* argp );
			
			void SetNonblocking  ()  { Nitrogen::OTSetNonBlocking( itsEndpoint ); }
			void ClearNonblocking()  { Nitrogen::OTSetBlocking   ( itsEndpoint ); }
			
			void Bind( const sockaddr& local, socklen_t len );
			
			void Listen( int backlog );
			
			std::auto_ptr< IOHandle > Accept( sockaddr& client, socklen_t& len );
			
			void Connect( const sockaddr& server, socklen_t len );
			
			const SocketAddress& GetSockName() const  { return itsSocketAddress; }
			const SocketAddress& GetPeerName() const  { return itsPeerAddress;   }
			
			void ShutdownReading()  {}
			void ShutdownWriting();
	};
	
}

#endif

