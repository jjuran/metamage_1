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
			int                                      itsBacklog;
			SocketAddress                            itsSocketAddress;
			SocketAddress                            itsPeerAddress;
			bool                                     itIsBound;
			bool                                     itIsListener;
			bool                                     itHasSentFIN;
			bool                                     itHasReceivedFIN;
			bool                                     itHasReceivedRST;
		
		public:
			OTSocket( bool nonblocking = false );
			
			~OTSocket();
			
			void ReceiveDisconnect();
			void ReceiveOrderlyDisconnect();
			
			bool RepairListener();
			
			unsigned int SysPoll();
			
			ssize_t SysRead( char* data, std::size_t byteCount );
			
			ssize_t SysWrite( const char* data, std::size_t byteCount );
			
			//void IOCtl( unsigned long request, int* argp );
			
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

