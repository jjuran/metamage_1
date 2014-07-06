/*
	fifo.cc
	-------
*/

#include "relix/fs/fifo.hh"

// Standard C++
#include <map>

// plus
#include "plus/conduit.hh"
#include "plus/string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/enum/poll_result.hh"
#include "vfs/functions/pathname.hh"
#include "vfs/filehandle/functions/nonblocking.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/stream_method_set.hh"

// relix-kernel
#include "relix/api/broken_pipe.hh"
#include "relix/api/try_again.hh"


namespace relix
{
	
	namespace p7 = poseven;
	
	
	struct fifo_connection
	{
		vfs::filehandle*  reader;
		vfs::filehandle*  writer;
		
		fifo_connection() : reader(), writer()
		{
		}
	};
	
	typedef std::map< plus::string, fifo_connection > Map;
	
	typedef Map::iterator  Iter;
	
	static Map the_pending_connections;
	
	struct fifo_extra
	{
		const plus::string*  key;
		fifo_connection*     connection;
		
		plus::conduit* conduit;
	};
	
	
	static unsigned fifo_poll( vfs::filehandle* that )
	{
		fifo_extra& extra = *(fifo_extra*) that->extra();
		
		const bool reading = (that->get_flags() & O_ACCMODE) == O_RDONLY;
		
		const unsigned r  = vfs::Poll_read;
		const unsigned w  = vfs::Poll_write;
		const unsigned rw = r|w;
		
		if ( extra.conduit == NULL )
		{
			return rw;
		}
		
		return reading ? (extra.conduit->is_readable() ? rw : w)
		               : (extra.conduit->is_writable() ? rw : r);
	}
	
	static ssize_t fifo_read( vfs::filehandle* that, char* buffer, size_t n )
	{
		fifo_extra& extra = *(fifo_extra*) that->extra();
		
		const bool reading = (that->get_flags() & O_ACCMODE) == O_RDONLY;
		
		if ( !reading )
		{
			p7::throw_errno( EBADF );
		}
		
		while ( extra.conduit == NULL )
		{
			try_again( is_nonblocking( *that ) );
		}
		
		return extra.conduit->read( buffer, n, is_nonblocking( *that ), &try_again );
	}
	
	static ssize_t fifo_write( vfs::filehandle* that, const char* buffer, size_t n )
	{
		fifo_extra& extra = *(fifo_extra*) that->extra();
		
		const bool reading = (that->get_flags() & O_ACCMODE) == O_RDONLY;
		
		if ( reading )
		{
			p7::throw_errno( EBADF );
		}
		
		return extra.conduit->write( buffer,
		                             n,
		                             is_nonblocking( *that ),
		                             &try_again,
		                             &broken_pipe );
	}
	
	
	const vfs::stream_method_set fifo_stream_methods =
	{
		&fifo_poll,
		&fifo_read,
		&fifo_write,
	};
	
	static const vfs::filehandle_method_set fifo_methods =
	{
		NULL,
		NULL,
		&fifo_stream_methods,
	};
	
	static void close_fifo( vfs::filehandle* that )
	{
		fifo_extra& extra = *(fifo_extra*) that->extra();
		
		if ( extra.connection == NULL )
		{
			return;
		}
		
		const bool reading = (that->get_flags() & O_ACCMODE) == O_RDONLY;
		
		if ( extra.conduit )
		{
			if ( reading )
			{
				extra.conduit->close_egress();
			}
			else
			{
				extra.conduit->close_ingress();
			}
			
			intrusive_ptr_release( extra.conduit );
		}
		
		fifo_connection& connection = *extra.connection;
		
		vfs::filehandle*& us   = reading ? connection.reader : connection.writer;
		vfs::filehandle*& them = reading ? connection.writer : connection.reader;
		
		us = NULL;
		
		if ( them == NULL )
		{
			the_pending_connections.erase( *extra.key );
		}
	}
	
	static vfs::filehandle* new_fifo_endpoint_( const vfs::node&  that,
	                                            int               flags )
	{
		vfs::filehandle* result = new vfs::filehandle( &that,
		                                               flags,
		                                               &fifo_methods,
		                                               sizeof (fifo_extra),
		                                               &close_fifo );
		
		return result;
	}
	
	vfs::filehandle_ptr open_fifo( const vfs::node*  that,
	                               int               flags )
	{
		plus::string path = pathname( *that );
		
		bool has_reader = false;
		bool has_writer = false;
		
		Iter it = the_pending_connections.find( path );
		
		if ( it != the_pending_connections.end() )
		{
			has_reader = it->second.reader != NULL;
			has_writer = it->second.writer != NULL;
		}
		
		const bool reading = (flags & O_ACCMODE) == O_RDONLY;
		
		const bool nonblocking = flags & O_NONBLOCK;
		
		if ( nonblocking  &&  !reading  &&  !has_reader )
		{
			p7::throw_errno( ENXIO );
		}
		
		if ( it == the_pending_connections.end() )
		{
			typedef Map::value_type Value;
			
			it = the_pending_connections.insert( Value( path, fifo_connection() ) ).first;
		}
		
		const plus::string&  key        = it->first;
		fifo_connection&     connection = it->second;
		
		vfs::filehandle*& us   = reading ? connection.reader : connection.writer;
		vfs::filehandle*& them = reading ? connection.writer : connection.reader;
		
		vfs::filehandle_ptr result;
		
		if ( us == NULL )
		{
			us = new_fifo_endpoint_( *that, flags );
			
			fifo_extra& extra = *(fifo_extra*) us->extra();
			
			extra.key        = &key;
			extra.connection = &connection;
			
			extra.conduit = NULL;
		}
		
		result = us;
		
		if ( them != NULL )
		{
			boost::intrusive_ptr< plus::conduit > conduit( new plus::conduit );
			
			fifo_extra& ours   = *(fifo_extra*) us  ->extra();
			fifo_extra& theirs = *(fifo_extra*) them->extra();
			
			ours.conduit = theirs.conduit = conduit.get();
			
			intrusive_ptr_add_ref( conduit.get() );
			intrusive_ptr_add_ref( conduit.get() );
		}
		else if ( !nonblocking )
		{
			while ( them == NULL )
			{
				try_again( false );
			}
		}
		
		return result;
	}
	
}
