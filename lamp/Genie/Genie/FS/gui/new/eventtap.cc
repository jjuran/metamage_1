/*
	gui/new/eventtap.cc
	-------------------
*/

#include "Genie/FS/gui/new/eventtap.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __EVENTS__
#include <Events.h>
#endif

// POSIX
#include <sys/stat.h>

// poseven
#include "poseven/types/errno_t.hh"

// Pedestal
#include "Pedestal/View.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle_ptr.hh"
#include "vfs/node.hh"
#include "vfs/enum/poll_result.hh"
#include "vfs/filehandle/functions/nonblocking.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/stream_method_set.hh"
#include "vfs/filehandle/primitives/get_file.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/node/types/fixed_dir.hh"

// relix
#include "relix/api/try_again.hh"

// Genie
#include "Genie/FS/Views.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	class ring_buffer
	{
		private:
			static const size_t alloc_size = 1024;
			
			unsigned char its_data[ alloc_size ];
			
			unsigned char* its_head;
			unsigned char* its_tail;
			
			// non-copyable
			ring_buffer           ( const ring_buffer& );
			ring_buffer& operator=( const ring_buffer& );
		
		public:
			ring_buffer()
			:
				its_head( its_data ),
				its_tail( its_data )
			{
			}
			
			bool empty() const  { return its_head == its_tail; }
			
			ssize_t read( char* buffer, size_t n );
			
			void write( const unsigned char* buffer, size_t n );
	};
	
	ssize_t ring_buffer::read( char* buffer, size_t n )
	{
		if ( empty() )
		{
			return 0;
		}
		
		size_t len = its_head[ 0 ];
		
		++len;
		
		if ( its_head + len > its_tail )
		{
			p7::throw_errno( EIO );
		}
		
		if ( n < len )
		{
			p7::throw_errno( EINVAL );
		}
		
		memcpy( buffer, its_head, len );
		
		its_head += len;
		
		if ( empty() )
		{
			its_head = its_data;
			its_tail = its_data;
		}
		
		return len;
	}
	
	void ring_buffer::write( const unsigned char* buffer, size_t n )
	{
		unsigned char* end = its_data + alloc_size;
		
		const size_t tail_room = end - its_tail;
		
		if ( n > tail_room )
		{
			const size_t head_room = its_head - its_data;
			
			if ( head_room < alloc_size / 2 )
			{
				p7::throw_errno( ENOSPC );
			}
			
			/*
				FIXME:  Assert that the write doesn't exceed what we know
				to be at least half the buffer.
			*/
			
			memcpy( its_data, its_head, its_tail - its_head );
			
			its_head -= head_room;
			its_tail -= head_room;
		}
		else
		{
			memcpy( its_tail, buffer, n );
			
			its_tail += n;
		}
	}
	
	struct eventtap_stream_char_buffer
	{
		uint8_t len;
		uint8_t ascii;
	};
	
	struct eventtap_stream_client
	{
		ring_buffer* buffer;
	};
	
	struct eventtap_stream_server : vfs::fixed_dir_extra
	{
		eventtap_stream_client* client;
	};
	
	const size_t eventtap_annex_size = sizeof (eventtap_stream_server)
	                                 - sizeof (vfs::fixed_dir_extra);
	
	
	struct non_ascii_char {};
	
	class eventtap_handler : public Ped::View
	{
		private:
			const vfs::node* itsKey;
		
		public:
			eventtap_handler( const vfs::node* key ) : itsKey( key )
			{
			}
			
			bool KeyDown( const EventRecord& event );
	};
	
	bool eventtap_handler::KeyDown( const EventRecord& event )
	{
		eventtap_stream_server& extra = *(eventtap_stream_server*) itsKey->extra();
		
		const signed char c = char( event.message );
		
		if ( c < 0 )
		{
			throw non_ascii_char();
		}
		
		eventtap_stream_char_buffer buffer =
		{
			1,
			c
		};
		
		if ( extra.client )
		{
			extra.client->buffer->write( &buffer.len, sizeof buffer );
		}
		
		return true;
	}
	
	static boost::intrusive_ptr< Ped::View > CreateView( const vfs::node* delegate )
	{
		eventtap_stream_server& extra = *(eventtap_stream_server*) delegate->extra();
		
		extra.client = NULL;
		
		return new eventtap_handler( delegate );
	}
	
	
	typedef eventtap_stream_client eventtap_stream_tape_extra;
	
	static unsigned eventtap_stream_poll( vfs::filehandle* that )
	{
		eventtap_stream_client& extra = *(eventtap_stream_client*) that->extra();
		
		return !extra.buffer->empty() ? vfs::Poll_read | vfs::Poll_write : vfs::Poll_write;
	}
	
	static ssize_t eventtap_stream_read( vfs::filehandle* that, char* buffer, size_t n )
	{
		eventtap_stream_client& extra = *(eventtap_stream_client*) that->extra();
		
		while ( extra.buffer->empty() )
		{
			relix::try_again( is_nonblocking( *that ) );
		}
		
		return extra.buffer->read( buffer, n );
	}
	
	static const vfs::stream_method_set eventtap_stream_stream_methods =
	{
		&eventtap_stream_poll,
		&eventtap_stream_read,
	};
	
	static const vfs::filehandle_method_set eventtap_stream_tape_methods =
	{
		NULL,
		NULL,
		&eventtap_stream_stream_methods,
	};
	
	static void destroy_eventtap_stream_tape( vfs::filehandle* that )
	{
		eventtap_stream_client& extra = *(eventtap_stream_client*) that->extra();
		
		((eventtap_stream_server*) get_file( *that )->owner()->extra())->client = NULL;
		
		delete extra.buffer;
	}
	
	static vfs::filehandle_ptr eventtap_stream_open( const vfs::node* that, int flags, mode_t mode )
	{
		vfs::filehandle_ptr result = new vfs::filehandle( that,
		                                                  flags,
		                                                  &eventtap_stream_tape_methods,
		                                                  sizeof (eventtap_stream_tape_extra),
		                                                  &destroy_eventtap_stream_tape );
		
		eventtap_stream_client& extra = *(eventtap_stream_client*) result->extra();
		
		extra.buffer = NULL;
		extra.buffer = new ring_buffer();
		
		((eventtap_stream_server*) that->owner()->extra())->client = &extra;
		
		return result;
	}
	
	static const vfs::data_method_set eventtap_stream_data_methods =
	{
		&eventtap_stream_open
	};
	
	static const vfs::node_method_set eventtap_stream_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&eventtap_stream_data_methods
	};
	
	
	static vfs::node_ptr eventtap_stream_factory( const vfs::node*     parent,
	                                              const plus::string&  name,
	                                              const void*          args )
	{
		return new vfs::node( parent, name, S_IFCHR | 0400, &eventtap_stream_methods );
	}
	
	static const vfs::fixed_mapping local_mappings[] =
	{
		{ "stream", &eventtap_stream_factory },
		
		{ NULL, NULL }
	};
	
	vfs::node_ptr new_eventtap( const vfs::node*     parent,
	                            const plus::string&  name,
	                            const void*          args )
	{
		return New_new_view( parent,
		                     name,
		                     &CreateView,
		                     local_mappings,
		                     NULL,
		                     eventtap_annex_size );
	}
	
}
