/*
	dynamic_group.hh
	----------------
*/

#ifndef VFS_NODE_TYPES_DYNAMICGROUP_HH
#define VFS_NODE_TYPES_DYNAMICGROUP_HH

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace vfs
{
	
	node_ptr dynamic_group_factory( const node*          parent,
	                                const plus::string&  name,
	                                const void*          args );
	
}

#endif
