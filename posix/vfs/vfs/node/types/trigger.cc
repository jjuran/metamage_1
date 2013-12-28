/*
	trigger.cc
	----------
*/

#include "vfs/node/types/trigger.hh"

// POSIX
#include <sys/stat.h>

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/stream_method_set.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/primitives/touch.hh"


namespace vfs
{
	
	static ssize_t trigger_write( filehandle* that, const char* buffer, size_t n )
	{
		touch( *that->GetFile() );
		
		return n;
	}
	
	static const stream_method_set trigger_stream_methods =
	{
		NULL,
		NULL,
		&trigger_write,
	};
	
	static const filehandle_method_set trigger_io_methods =
	{
		NULL,
		NULL,
		&trigger_stream_methods,
	};
	
	
	static void trigger_touch( const node* that )
	{
		if ( trigger_function f = *(trigger_function*) that->extra() )
		{
			f( that );
			
			return;
		}
	}
	
	static filehandle_ptr trigger_open( const node* that, int flags, mode_t mode );
	
	static const data_method_set trigger_data_methods =
	{
		&trigger_open
	};
	
	static const node_method_set trigger_methods =
	{
		NULL,
		NULL,
		&trigger_touch,
		NULL,
		NULL,
		NULL,
		&trigger_data_methods
	};
	
	static filehandle_ptr trigger_open( const node* that, int flags, mode_t mode )
	{
		return new filehandle( that, flags, &trigger_io_methods );
	}
	
	
	node_ptr trigger_factory( const node*     parent,
	                          const plus::string&  name,
	                          const void*          args )
	{
		node* result = new node( parent,
		                         name,
		                         S_IFCHR | 0200,
		                         &trigger_methods,
		                         sizeof (trigger_extra) );
		
		trigger_extra& extra = *(trigger_extra*) result->extra();
		
		extra = *(trigger_extra*) args;
		
		return result;
	}
	
}

