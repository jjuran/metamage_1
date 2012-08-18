/*	===============
 *	PairedSocket.cc
 *	===============
 */

#include "Genie/IO/PairedSocket.hh"

// POSIX
#include <sys/socket.h>

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/OSUtils.hh"

// Genie
#include "Genie/api/signals.hh"
#include "Genie/api/yield.hh"
#include "Genie/IO/SocketStream.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	class PairedSocket : public SocketHandle
	{
		private:
			boost::intrusive_ptr< conduit >  itsInput;
			boost::intrusive_ptr< conduit >  itsOutput;
		
		public:
			PairedSocket( boost::intrusive_ptr< conduit >  input,
			              boost::intrusive_ptr< conduit >  output,
			              bool                             nonblocking );
			
			~PairedSocket();
			
			unsigned int SysPoll()
			{
				return   kPollRead  * itsInput->is_readable()
				       | kPollWrite * itsOutput->is_writable();
			}
			
			ssize_t SysRead( char* data, std::size_t byteCount )
			{
				return itsInput->read( data, byteCount, IsNonblocking(), &try_again );
			}
			
			ssize_t SysWrite( const char* data, std::size_t byteCount )
			{
				return itsOutput->write( data, byteCount, IsNonblocking(), &try_again, &broken_pipe );
			}
			
			//void IOCtl( unsigned long request, int* argp );
			
			void Bind( const sockaddr& local, socklen_t len );
			
			void Listen( int backlog );
			
			IOPtr Accept( sockaddr& client, socklen_t& len );
			
			void Connect( const sockaddr& server, socklen_t len );
			
			const SocketAddress& GetSockName() const;
			const SocketAddress& GetPeerName() const;
			
			void ShutdownReading();
			void ShutdownWriting();
	};
	
	
	PairedSocket::PairedSocket( boost::intrusive_ptr< conduit >  input,
			                    boost::intrusive_ptr< conduit >  output,
			                    bool                             nonblocking )
	:
		SocketHandle( nonblocking ),
		itsInput ( input  ),
		itsOutput( output )
	{
	}
	
	PairedSocket::~PairedSocket()
	{
		ShutdownReading();
		ShutdownWriting();
	}
	
	void PairedSocket::Bind( const sockaddr& local, socklen_t len )
	{
		p7::throw_errno( EINVAL );
	}
	
	void PairedSocket::Listen( int backlog )
	{
		p7::throw_errno( EINVAL );
	}
	
	IOPtr PairedSocket::Accept( sockaddr& client, socklen_t& len )
	{
		p7::throw_errno( EINVAL );
		
		throw;
	}
	
	void PairedSocket::Connect( const sockaddr& server, socklen_t len )
	{
		p7::throw_errno( EINVAL );
	}
	
	const SocketAddress& PairedSocket::GetSockName() const
	{
		p7::throw_errno( EINVAL );
		
		throw;
	}
	
	const SocketAddress& PairedSocket::GetPeerName() const
	{
		p7::throw_errno( EINVAL );
		
		throw;
	}
	
	void PairedSocket::ShutdownReading()
	{
		itsInput->close_egress();
	}
	
	void PairedSocket::ShutdownWriting()
	{
		itsOutput->close_ingress();
	}
	
	
	IOPtr
	//
	NewPairedSocket( const boost::intrusive_ptr< conduit >&  input,
	                 const boost::intrusive_ptr< conduit >&  output,
	                 bool                                    nonblocking )
	{
		return new PairedSocket( input, output, nonblocking );
	}
	
}

