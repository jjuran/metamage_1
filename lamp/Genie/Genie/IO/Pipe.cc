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
			
			plus::conduit& get_conduit() const  { return *itsConduit; }
	};
	
	class PipeOutHandle : public StreamHandle
	{
		private:
			boost::intrusive_ptr< plus::conduit > itsConduit;
		
		public:
			PipeOutHandle( const boost::intrusive_ptr< plus::conduit >&  conduit,
			               bool                                          nonblocking );
			
			~PipeOutHandle();
			
			plus::conduit& get_conduit() const  { return *itsConduit; }
	};
	
	static unsigned pipein_poll( vfs::filehandle* that )
	{
		plus::conduit& conduit = static_cast< PipeInHandle& >( *that ).get_conduit();
		
		return + vfs::Poll_read
		       | vfs::Poll_write * conduit.is_writable();
	}
	
	static ssize_t pipein_read( vfs::filehandle* that, char* buffer, size_t n )
	{
		plus::conduit& conduit = static_cast< PipeInHandle& >( *that ).get_conduit();
		
		return conduit.read( buffer, n, is_nonblocking( *that ), &try_again );
	}
	
	static unsigned pipeout_poll( vfs::filehandle* that )
	{
		plus::conduit& conduit = static_cast< PipeOutHandle& >( *that ).get_conduit();
		
		return + vfs::Poll_read * conduit.is_readable()
		       | vfs::Poll_write;
	}
	
	static ssize_t pipeout_write( vfs::filehandle* that, const char* buffer, size_t n )
	{
		plus::conduit& conduit = static_cast< PipeOutHandle& >( *that ).get_conduit();
		
		return conduit.write( buffer, n, is_nonblocking( *that ), &try_again, &broken_pipe );
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
	
	pipe_ends new_pipe( int nonblock )
	{
		boost::intrusive_ptr< plus::conduit > conduit( new plus::conduit );
		
		pipe_ends result;
		
		result.writer.reset( new PipeInHandle ( conduit, nonblock ) );
		result.reader.reset( new PipeOutHandle( conduit, nonblock ) );
		
		return result;
	}
	
}

