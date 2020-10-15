/*
	pipe.cc
	-------
*/

#include "relix/fs/pipe.hh"

// POSIX
#include <fcntl.h>

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// vfs
#include "vfs/conduit.hh"
#include "vfs/filehandle.hh"
#include "vfs/enum/poll_result.hh"
#include "vfs/filehandle/functions/nonblocking.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/stream_method_set.hh"

// relix
#include "relix/api/broken_pipe.hh"
#include "relix/api/try_again.hh"


namespace relix
{
	
	enum pipe_end_type
	{
		Pipe_reader,
		Pipe_writer,
	};
	
	struct pipe_end_extra
	{
		vfs::conduit*  conduit;
		pipe_end_type  type;
	};
	
	static unsigned pipein_poll( vfs::filehandle* that )
	{
		pipe_end_extra& extra = *(pipe_end_extra*) that->extra();
		
		vfs::conduit& conduit = *extra.conduit;
		
		return + vfs::Poll_read
		       | vfs::Poll_write * conduit.is_writable();
	}
	
	static ssize_t pipein_read( vfs::filehandle* that, char* buffer, size_t n )
	{
		pipe_end_extra& extra = *(pipe_end_extra*) that->extra();
		
		vfs::conduit& conduit = *extra.conduit;
		
		return conduit.read( buffer, n, is_nonblocking( *that ), &try_again );
	}
	
	static unsigned pipeout_poll( vfs::filehandle* that )
	{
		pipe_end_extra& extra = *(pipe_end_extra*) that->extra();
		
		vfs::conduit& conduit = *extra.conduit;
		
		return + vfs::Poll_read * conduit.is_readable()
		       | vfs::Poll_write;
	}
	
	static ssize_t pipeout_write( vfs::filehandle* that, const char* buffer, size_t n )
	{
		pipe_end_extra& extra = *(pipe_end_extra*) that->extra();
		
		vfs::conduit& conduit = *extra.conduit;
		
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
	
	
	static inline const vfs::filehandle_method_set& methods_for_end( pipe_end_type type )
	{
		return type == Pipe_reader ? pipein_methods
		                           : pipeout_methods;
	}
	
	static
	void destroy_pipe_end( vfs::filehandle* that );
	
	static
	vfs::filehandle* new_pipe_end( vfs::conduit&  conduit,
	                               int            open_flags,
	                               pipe_end_type  end_type )
	{
		const vfs::filehandle_method_set& methods = methods_for_end( end_type );
		
		vfs::filehandle* result = new vfs::filehandle( NULL,
		                                               open_flags,
		                                               &methods,
		                                               sizeof (pipe_end_extra),
		                                               &destroy_pipe_end );
		
		pipe_end_extra& extra = *(pipe_end_extra*) result->extra();
		
		extra.conduit = &conduit;
		extra.type    = end_type;
		
		intrusive_ptr_add_ref( extra.conduit );
		
		return result;
	}
	
	static
	void destroy_pipe_end( vfs::filehandle* that )
	{
		pipe_end_extra& extra = *(pipe_end_extra*) that->extra();
		
		if ( extra.type == Pipe_reader )
		{
			extra.conduit->close_egress();
		}
		else
		{
			extra.conduit->close_ingress();
		}
		
		intrusive_ptr_release( extra.conduit );
	}
	
	pipe_ends new_pipe( int nonblock )
	{
		boost::intrusive_ptr< vfs::conduit > conduit( new vfs::conduit );
		
		pipe_ends result;
		
		const int r_flags = nonblock ? O_RDONLY | O_NONBLOCK
		                             : O_RDONLY;
		
		const int w_flags = nonblock ? O_WRONLY | O_NONBLOCK
		                             : O_WRONLY;
		
		result.writer.reset( new_pipe_end( *conduit, w_flags, Pipe_writer ) );
		result.reader.reset( new_pipe_end( *conduit, r_flags, Pipe_reader ) );
		
		return result;
	}
	
}
