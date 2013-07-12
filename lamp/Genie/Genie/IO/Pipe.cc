/*	=======
 *	Pipe.cc
 *	=======
 */

#include "Genie/IO/Pipe.hh"

// POSIX
#include <fcntl.h>

// Genie
#include "Genie/api/signals.hh"
#include "Genie/api/yield.hh"


namespace Genie
{
	
	PipeInHandle::PipeInHandle( const boost::intrusive_ptr< plus::conduit >&  conduit,
	                            bool                                          nonblocking )
	:
		StreamHandle( nonblocking ? O_WRONLY | O_NONBLOCK
		                          : O_WRONLY ),
		itsConduit( conduit )
	{
	}
	
	PipeInHandle::~PipeInHandle()
	{
		itsConduit->close_ingress();
	}
	
	unsigned PipeInHandle::SysPoll()
	{
		return + kPollRead
		       | kPollWrite * itsConduit->is_writable();
	}
	
	ssize_t PipeInHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		return itsConduit->write( data, byteCount, IsNonblocking(), &try_again, &broken_pipe );
	}
	
	
	PipeOutHandle::PipeOutHandle( const boost::intrusive_ptr< plus::conduit >&  conduit,
	                              bool                                          nonblocking )
	:
		StreamHandle( nonblocking ? O_RDONLY | O_NONBLOCK
		                          : O_RDONLY ),
		itsConduit( conduit )
	{
	}
	
	PipeOutHandle::~PipeOutHandle()
	{
		itsConduit->close_egress();
	}
	
	unsigned PipeOutHandle::SysPoll()
	{
		return + kPollRead * itsConduit->is_readable()
		       | kPollWrite;
	}
	
	ssize_t PipeOutHandle::SysRead( char* data, std::size_t byteCount )
	{
		return itsConduit->read( data, byteCount, IsNonblocking(), &try_again );
	}
	
}

