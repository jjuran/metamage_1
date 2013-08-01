/*	=======
 *	Pipe.cc
 *	=======
 */

#include "Genie/IO/Pipe.hh"

// POSIX
#include <fcntl.h>

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// plus
#include "plus/conduit.hh"

// Genie
#include "Genie/api/signals.hh"
#include "Genie/api/yield.hh"
#include "Genie/IO/Stream.hh"


namespace Genie
{
	
	class PipeInHandle : public StreamHandle
	{
		private:
			boost::intrusive_ptr< plus::conduit > itsConduit;
		
		public:
			PipeInHandle( const boost::intrusive_ptr< plus::conduit >&  conduit,
			              bool                                          nonblocking );
			
			~PipeInHandle();
			
			unsigned int SysPoll();
			
			ssize_t SysWrite( const char* data, std::size_t byteCount );
	};
	
	class PipeOutHandle : public StreamHandle
	{
		private:
			boost::intrusive_ptr< plus::conduit > itsConduit;
		
		public:
			PipeOutHandle( const boost::intrusive_ptr< plus::conduit >&  conduit,
			               bool                                          nonblocking );
			
			~PipeOutHandle();
			
			unsigned int SysPoll();
			
			ssize_t SysRead( char* data, std::size_t byteCount );
	};
	
	
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
	
	pipe_ends new_pipe( int nonblock )
	{
		boost::intrusive_ptr< plus::conduit > conduit( new plus::conduit );
		
		pipe_ends result;
		
		result.writer.reset( new PipeInHandle ( conduit, nonblock ) );
		result.reader.reset( new PipeOutHandle( conduit, nonblock ) );
		
		return result;
	}
	
}

