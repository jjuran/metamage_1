/*	==========
 *	Trigger.cc
 *	==========
 */

#include "Genie/FS/Trigger.hh"

// POSIX
#include <sys/stat.h>

// vfs
#include "vfs/node.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/stream_method_set.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/primitives/touch.hh"

// Genie
#include "Genie/IO/Stream.hh"


namespace Genie
{
	
	static ssize_t trigger_write( vfs::filehandle* that, const char* buffer, size_t n )
	{
		touch( *that->GetFile() );
		
		return n;
	}
	
	static const vfs::stream_method_set trigger_stream_methods =
	{
		NULL,
		NULL,
		&trigger_write,
	};
	
	static const vfs::filehandle_method_set trigger_io_methods =
	{
		NULL,
		NULL,
		&trigger_stream_methods,
	};
	
	
	static void trigger_touch( const vfs::node* that )
	{
		if ( trigger_function f = *(trigger_function*) that->extra() )
		{
			f( that );
			
			return;
		}
	}
	
	static vfs::filehandle_ptr trigger_open( const vfs::node* that, int flags, mode_t mode );
	
	static const vfs::data_method_set trigger_data_methods =
	{
		&trigger_open
	};
	
	static const vfs::node_method_set trigger_methods =
	{
		NULL,
		NULL,
		&trigger_touch,
		NULL,
		NULL,
		NULL,
		&trigger_data_methods
	};
	
	class TriggerHandle : public StreamHandle
	{
		public:
			TriggerHandle( const vfs::node&  file,
			               int               flags )
			:
				StreamHandle( &file, flags, &trigger_io_methods )
			{
			}
	};
	
	static vfs::filehandle_ptr trigger_open( const vfs::node* that, int flags, mode_t mode )
	{
		return new TriggerHandle( *that, flags );
	}
	
	
	vfs::node_ptr trigger_factory( const vfs::node*     parent,
	                               const plus::string&  name,
	                               const void*          args )
	{
		vfs::node* result = new vfs::node( parent,
		                                   name,
		                                   S_IFCHR | 0200,
		                                   &trigger_methods,
		                                   sizeof (trigger_extra) );
		
		trigger_extra& extra = *(trigger_extra*) result->extra();
		
		extra = *(trigger_extra*) args;
		
		return result;
	}
	
}

