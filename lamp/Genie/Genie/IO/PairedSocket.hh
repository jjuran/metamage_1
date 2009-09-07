/*	===============
 *	PairedSocket.hh
 *	===============
 */

#ifndef GENIE_IO_PAIREDSOCKET_HH
#define GENIE_IO_PAIREDSOCKET_HH

// Debug
#include "debug/boost_assert.hh"

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
			boost::shared_ptr< Conduit >  itsInput;
			boost::shared_ptr< Conduit >  itsOutput;
		
		public:
			PairedSocket( boost::shared_ptr< Conduit >  input,
			              boost::shared_ptr< Conduit >  output,
			              bool                          nonblocking );
			
			~PairedSocket();
			
			unsigned int SysPoll()
			{
				return   kPollRead  * itsInput->IsReadable()
				       | kPollWrite * itsOutput->IsWritable();
			}
			
			ssize_t SysRead( char* data, std::size_t byteCount )
			{
				return itsInput->Read( data, byteCount, IsNonblocking() );
			}
			
			ssize_t SysWrite( const char* data, std::size_t byteCount )
			{
				return itsOutput->Write( data, byteCount, IsNonblocking() );
			}
			
			//void IOCtl( unsigned long request, int* argp );
			
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

