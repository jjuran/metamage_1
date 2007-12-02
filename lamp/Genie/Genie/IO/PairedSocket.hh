/*	===============
 *	PairedSocket.hh
 *	===============
 */

#ifndef GENIE_IO_PAIREDSOCKET_HH
#define GENIE_IO_PAIREDSOCKET_HH

// Boost
#include <boost/shared_ptr.hpp>

// POSIX
#include <sys/socket.h>

// Genie
#include "Genie/IO/Conduit.hh"
#include "Genie/IO/SocketStream.hh"


namespace Genie
{
	
	class PairedSocket : public SocketHandle
	{
		private:
			boost::shared_ptr< Conduit > itsInput;
			boost::shared_ptr< Conduit > itsOutput;
			bool itIsBlocking;
		
		public:
			PairedSocket( boost::shared_ptr< Conduit >  input,
			              boost::shared_ptr< Conduit >  output );
			
			~PairedSocket();
			
			unsigned int SysPoll() const
			{
				return   kPollRead  * itsInput->IsReadable()
				       | kPollWrite * itsOutput->IsWritable();
			}
			
			int SysRead( char* data, std::size_t byteCount )
			{
				return itsInput->Read( data, byteCount, itIsBlocking );
			}
			
			int SysWrite( const char* data, std::size_t byteCount )
			{
				return itsOutput->Write( data, byteCount, itIsBlocking );
			}
			
			//void IOCtl( unsigned long request, int* argp );
			
			bool IsBlocking() const  { return itIsBlocking; }
			
			void SetBlocking   ()  { itIsBlocking = true;  }
			void SetNonBlocking()  { itIsBlocking = false; }
			
			void Bind( const sockaddr& local, socklen_t len );
			
			void Listen( int backlog );
			
			std::auto_ptr< IOHandle > Accept( sockaddr& client, socklen_t& len );
			
			void Connect( const sockaddr& server, socklen_t len );
			
			const SocketAddress& GetSockName() const;
			const SocketAddress& GetPeerName() const;
			
			void ShutdownReading();
			void ShutdownWriting();
	};
	
}

#endif

