/*
	new_static_symlink.cc
	---------------------
*/

#include "vfs/functions/new_static_symlink.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/node/types/symbolic_link.hh"


namespace vfs
{
	
	node_ptr new_static_symlink( const node*          parent,
	                             const plus::string&  name,
	                             const void*          params )
	{
		const char* target = (const char*) params;
		
		return new_symbolic_link( parent, name, target );
	}
	
}

