/*	=======
 *	Pipe.hh
 *	=======
 */

#ifndef GENIE_IO_PIPE_HH
#define GENIE_IO_PIPE_HH

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
			              bool                                          nonblocking )
			:
				StreamHandle( nonblocking ? O_WRONLY | O_NONBLOCK
				                          : O_WRONLY ),
				itsConduit( conduit )
			{
			}
			
			~PipeInHandle();
			
			unsigned int SysPoll()
			{
				return   kPollRead
				       | kPollWrite * itsConduit->is_writable();
			}
			
			ssize_t SysWrite( const char* data, std::size_t byteCount )
			{
				return itsConduit->write( data, byteCount, IsNonblocking(), &try_again, &broken_pipe );
			}
	};
	
	class PipeOutHandle : public StreamHandle
	{
		private:
			boost::intrusive_ptr< plus::conduit > itsConduit;
		
		public:
			PipeOutHandle( const boost::intrusive_ptr< plus::conduit >&  conduit,
			               bool                                          nonblocking )
			:
				StreamHandle( nonblocking ? O_RDONLY | O_NONBLOCK
				                          : O_RDONLY ),
				itsConduit( conduit )
			{
			}
			
			~PipeOutHandle();
			
			unsigned int SysPoll()
			{
				return   kPollRead * itsConduit->is_readable()
				       | kPollWrite;
			}
			
			ssize_t SysRead( char* data, std::size_t byteCount )
			{
				return itsConduit->read( data, byteCount, IsNonblocking(), &try_again );
			}
	};
	
}

#endif

