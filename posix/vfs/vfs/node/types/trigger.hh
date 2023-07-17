/*
	trigger.hh
	----------
*/

#ifndef VFS_NODE_TYPES_TRIGGER_HH
#define VFS_NODE_TYPES_TRIGGER_HH

// Standard C
#include <stdint.h>

// plus
#include "plus/string_fwd.hh"

//vfs
#include "vfs/node_ptr.hh"


namespace vfs
{
	
	typedef void (*trigger_function)( const node* );
	
	struct trigger_extra
	{
		trigger_function  function;
		intptr_t          data;
		
	};
	
	node_ptr trigger_factory( const node*          parent,
	                          const plus::string&  name,
	                          const void*          args );
	
}

#endif
