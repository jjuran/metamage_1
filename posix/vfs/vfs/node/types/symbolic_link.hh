/*
	symbolic_link.hh
	----------------
*/

#ifndef VFS_NODE_TYPES_SYMBOLICLINK_HH
#define VFS_NODE_TYPES_SYMBOLICLINK_HH

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_ptr.hh"
#include "vfs/methods/node_method_set.hh"


namespace vfs
{
	
	node_ptr new_symbolic_link( const node*          parent,
	                            const plus::string&  name,
	                            plus::string         target,
	                            remove_method        remove = NULL );
	
}

#endif

