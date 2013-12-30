/*
	union.hh
	--------
*/

#ifndef VFS_NODE_TYPES_UNION_HH
#define VFS_NODE_TYPES_UNION_HH

// plus
#include "plus/string_fwd.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace vfs
{
	
	node_ptr new_union_directory( const node*          parent,
	                              const plus::string&  name,
	                              const node*          top,
	                              const node*          bottom );
	
}

#endif

