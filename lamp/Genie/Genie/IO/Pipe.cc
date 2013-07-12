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

// vfs
#include "vfs/enum/poll_result.hh"
#include "vfs/filehandle/functions/nonblocking.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/stream_method_set.hh"

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
	
	static unsigned pipein_poll( vfs::filehandle* that )
	{
		return static_cast< PipeInHandle& >( *that ).SysPoll();
	}
	
	static ssize_t pipein_read( vfs::filehandle* that, char* buffer, size_t n )
	{
		return static_cast< PipeInHandle& >( *that ).SysRead( buffer, n );
	}
	
	static unsigned pipeout_poll( vfs::filehandle* that )
	{
		return static_cast< PipeOutHandle& >( *that ).SysPoll();
	}
	
	static ssize_t pipeout_write( vfs::filehandle* that, const char* buffer, size_t n )
	{
		return static_cast< PipeOutHandle& >( *that ).SysWrite( buffer, n );
	}
	
	static const vfs::stream_method_set pipein_stream_methods =
	{
		&pipein_poll,
		&pipein_read,
	};
	
	static const vfs::stream_method_set pipeout_stream_methods =
	{
		&pipeout_poll,
		NULL,
		&pipeout_write,
	};
	
	static const vfs::filehandle_method_set pipein_methods =
	{
		NULL,
		NULL,
		&pipein_stream_methods,
	};
	
	static const vfs::filehandle_method_set pipeout_methods =
	{
		NULL,
		NULL,
		&pipeout_stream_methods,
	};
	
	
	PipeInHandle::PipeInHandle( const boost::intrusive_ptr< plus::conduit >&  conduit,
	                            bool                                          nonblocking )
	:
		StreamHandle( nonblocking ? O_WRONLY | O_NONBLOCK
		                          : O_WRONLY,
		              &pipein_methods ),
		itsConduit( conduit )
	{
	}
	
	PipeInHandle::~PipeInHandle()
	{
		itsConduit->close_ingress();
	}
	
	unsigned PipeInHandle::SysPoll()
	{
		return + vfs::Poll_read
		       | vfs::Poll_write * itsConduit->is_writable();
	}
	
	ssize_t PipeInHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		return itsConduit->write( data, byteCount, is_nonblocking( *this ), &try_again, &broken_pipe );
	}
	
	
	PipeOutHandle::PipeOutHandle( const boost::intrusive_ptr< plus::conduit >&  conduit,
	                              bool                                          nonblocking )
	:
		StreamHandle( nonblocking ? O_RDONLY | O_NONBLOCK
		                          : O_RDONLY,
		              &pipeout_methods ),
		itsConduit( conduit )
	{
	}
	
	PipeOutHandle::~PipeOutHandle()
	{
		itsConduit->close_egress();
	}
	
	unsigned PipeOutHandle::SysPoll()
	{
		return + vfs::Poll_read * itsConduit->is_readable()
		       | vfs::Poll_write;
	}
	
	ssize_t PipeOutHandle::SysRead( char* data, std::size_t byteCount )
	{
		return itsConduit->read( data, byteCount, is_nonblocking( *this ), &try_again );
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

