/*	===============
 *	PairedSocket.cc
 *	===============
 */

#include "Genie/IO/PairedSocket.hh"

// POSIX
#include <fcntl.h>
#include <sys/socket.h>

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/socket_method_set.hh"

// Nitrogen
#include "Nitrogen/OSUtils.hh"

// Genie
#include "Genie/api/signals.hh"
#include "Genie/api/yield.hh"
#include "Genie/IO/Stream.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	class PairedSocket : public StreamHandle
	{
		private:
			boost::intrusive_ptr< plus::conduit >  itsInput;
			boost::intrusive_ptr< plus::conduit >  itsOutput;
		
		public:
			PairedSocket( boost::intrusive_ptr< plus::conduit >  input,
			              boost::intrusive_ptr< plus::conduit >  output,
			              bool                                   nonblocking );
			
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
			
			void ShutdownReading();
			void ShutdownWriting();
	};
	
	
	static void pairedsocket_shutdown( vfs::filehandle* sock, int how )
	{
		if ( how != SHUT_WR )
		{
			static_cast< PairedSocket& >( *sock ).ShutdownReading();
		}
		
		if ( how != SHUT_RD )
		{
			static_cast< PairedSocket& >( *sock ).ShutdownWriting();
		}
	}
	
	static const vfs::socket_method_set pairedsocket_socket_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		&pairedsocket_shutdown,
	};
	
	static const vfs::filehandle_method_set pairedsocket_methods =
	{
		NULL,
		&pairedsocket_socket_methods,
	};
	
	
	PairedSocket::PairedSocket( boost::intrusive_ptr< plus::conduit >  input,
			                    boost::intrusive_ptr< plus::conduit >  output,
			                    bool                                   nonblocking )
	:
		StreamHandle( nonblocking ? O_RDWR | O_NONBLOCK
		                          : O_RDWR,
		              &pairedsocket_methods ),
		itsInput ( input  ),
		itsOutput( output )
	{
	}
	
	PairedSocket::~PairedSocket()
	{
		ShutdownReading();
		ShutdownWriting();
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
	NewPairedSocket( const boost::intrusive_ptr< plus::conduit >&  input,
	                 const boost::intrusive_ptr< plus::conduit >&  output,
	                 bool                                          nonblocking )
	{
		return new PairedSocket( input, output, nonblocking );
	}
	
}

