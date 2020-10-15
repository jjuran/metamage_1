/*
	fifo.cc
	-------
*/

#include "relix/fs/fifo.hh"

// Standard C++
#include <map>

// plus
#include "plus/string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/conduit.hh"
#include "vfs/conduit_max_page_count.hh"
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
		bool              opened;  // both reader and writer opened
		
		fifo_connection() : reader(), writer(), opened()
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
		
		vfs::conduit*  conduit;
		fifo_update_f  update;
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
	
	static inline
	fifo_state fifo_state_for_conduit( const vfs::conduit& conduit )
	{
		return fifo_state_for_ratio( conduit.pages_used(),
		                             vfs::conduit_max_page_count );
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
		
		fifo_update_f update = extra.update;
		
		if ( update  &&  !extra.conduit->is_readable() )
		{
			update( *that, FIFO_underrun );
		}
		
		ssize_t result = extra.conduit->read( buffer, n, is_nonblocking( *that ), &try_again );
		
		if ( update )
		{
			vfs::conduit& conduit = *extra.conduit;
			
			bool eof = !extra.connection->writer  &&  conduit.pages_used() == 0;
			
			fifo_state state = eof                   ? FIFO_EOF
			                 : conduit.is_writable() ? fifo_state_for_conduit( conduit )
			                 :                         FIFO_overrun;
			
			update( *that, state );
		}
		
		return result;
	}
	
	static ssize_t fifo_write( vfs::filehandle* that, const char* buffer, size_t n )
	{
		fifo_extra& extra = *(fifo_extra*) that->extra();
		
		const bool reading = (that->get_flags() & O_ACCMODE) == O_RDONLY;
		
		if ( reading )
		{
			p7::throw_errno( EBADF );
		}
		
		fifo_update_f update = extra.update;
		
		if ( update  &&  !extra.conduit->is_writable() )
		{
			update( *that, FIFO_overrun );
		}
		
		ssize_t result = extra.conduit->write( buffer, n, is_nonblocking( *that ), &try_again, &broken_pipe );
		
		if ( update )
		{
			update( *that, fifo_state_for_conduit( *extra.conduit ) );
		}
		
		return result;
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
		
		fifo_update_f update = extra.update;
		
		if ( update )
		{
			fifo_state new_state =  connection.writer          ? FIFO_broken_pipe
			                     : !connection.reader          ? FIFO_closed
			                     : extra.conduit->pages_used() ? fifo_state()
			                     :                               FIFO_EOF;
			
			if ( new_state != fifo_state() )
			{
				update( *that, new_state );
			}
		}
		
		if ( them == NULL )
		{
			the_pending_connections.erase( *extra.key );
		}
	}
	
	static vfs::filehandle* new_fifo_endpoint_( const vfs::node&  that,
	                                            int               flags,
	                                            fifo_update_f     update )
	{
		vfs::filehandle* result = new vfs::filehandle( &that,
		                                               flags,
		                                               &fifo_methods,
		                                               sizeof (fifo_extra),
		                                               &close_fifo );
		
		fifo_extra& extra = *(fifo_extra*) result->extra();
		
		extra.update = update;
		
		return result;
	}
	
	vfs::filehandle_ptr open_fifo( const vfs::node*  that,
	                               int               flags,
	                               fifo_update_f     update )
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
			us = new_fifo_endpoint_( *that, flags, update );
			
			fifo_extra& extra = *(fifo_extra*) us->extra();
			
			extra.key        = &key;
			extra.connection = &connection;
			
			extra.conduit = NULL;
		}
		
		result = us;
		
		fifo_state new_state = FIFO_empty;
		
		if ( them != NULL )
		{
			boost::intrusive_ptr< vfs::conduit > conduit( new vfs::conduit );
			
			fifo_extra& ours   = *(fifo_extra*) us  ->extra();
			fifo_extra& theirs = *(fifo_extra*) them->extra();
			
			ours.conduit = theirs.conduit = conduit.get();
			
			intrusive_ptr_add_ref( conduit.get() );
			intrusive_ptr_add_ref( conduit.get() );
			
			connection.opened = true;
		}
		else if ( !nonblocking )
		{
			if ( update )
			{
				update( *us, reading ? FIFO_half_open_reading
				                     : FIFO_half_open_writing );
			}
			
			while ( ! connection.opened )
			{
				try_again( false );
			}
		}
		else
		{
			new_state = FIFO_EOF;
		}
		
		if ( update )
		{
			update( *us, new_state );
		}
		
		return result;
	}
	
}
