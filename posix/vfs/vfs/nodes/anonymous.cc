/*
	anonymous.cc
	------------
*/

#include "vfs/nodes/anonymous.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/node_method_set.hh"


namespace vfs
{
	
	struct handle_extra
	{
		filehandle* handle;
	};
	
	static filehandle_ptr anonymous_open( const node* it, int flags, mode_t mode )
	{
		handle_extra& extra = *(handle_extra*) it->extra();
		
		return extra.handle;
	}
	
	static const data_method_set anonymous_data_methods =
	{
		&anonymous_open
	};
	
	static const node_method_set anonymous_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&anonymous_data_methods
	};
	
	static void dispose_handle( const node* it )
	{
		handle_extra& extra = *(handle_extra*) it->extra();
		
		intrusive_ptr_release( extra.handle );
	}
	
	node_ptr new_anonymous_node( const plus::string&  name,
	                             mode_t               mode,
	                             filehandle*          handle )
	{
		node* result = new node( NULL,
		                         name,
		                         mode,
		                         &anonymous_methods,
		                         sizeof (handle_extra),
		                         &dispose_handle );
		
		handle_extra& extra = *(handle_extra*) result->extra();
		
		intrusive_ptr_add_ref( handle );
		
		extra.handle = handle;
		
		return result;
	}
	
}

