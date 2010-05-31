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
#include "Genie/IO/SocketStream.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	class PairedSocket : public SocketHandle
	{
		private:
			boost::intrusive_ptr< Conduit >  itsInput;
			boost::intrusive_ptr< Conduit >  itsOutput;
		
		public:
			PairedSocket( boost::intrusive_ptr< Conduit >  input,
			              boost::intrusive_ptr< Conduit >  output,
			              bool                             nonblocking );
			
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
	
	
	PairedSocket::PairedSocket( boost::intrusive_ptr< Conduit >  input,
			                    boost::intrusive_ptr< Conduit >  output,
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
	
	std::auto_ptr< IOHandle > PairedSocket::Accept( sockaddr& client, socklen_t& len )
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
		itsInput->CloseEgress();
	}
	
	void PairedSocket::ShutdownWriting()
	{
		itsOutput->CloseIngress();
	}
	
	
	boost::shared_ptr< IOHandle >
	//
	NewPairedSocket( const boost::intrusive_ptr< Conduit >&  input,
	                 const boost::intrusive_ptr< Conduit >&  output,
	                 bool                                    nonblocking )
	{
		return seize_ptr( new PairedSocket( input, output, nonblocking ) );
	}
	
}

