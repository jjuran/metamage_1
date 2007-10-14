/*	===============
 *	PairedSocket.cc
 *	===============
 */

#include "Genie/IO/PairedSocket.hh"

// Io
#include "io/io.hh"

// POSeven
#include "POSeven/Errno.hh"

// Nitrogen
#include "Nitrogen/OSUtils.h"

// Genie
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	PairedSocket::PairedSocket( boost::shared_ptr< Conduit >  input,
			                    boost::shared_ptr< Conduit >  output ) : itsInput ( input  ),
	                                                                     itsOutput( output )
	{
	}
	
	PairedSocket::~PairedSocket()
	{
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
	
}

