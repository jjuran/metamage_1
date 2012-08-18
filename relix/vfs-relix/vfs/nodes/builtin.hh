/*
	builtin.hh
	----------
*/

#ifndef VFS_NODES_BUILTIN_HH
#define VFS_NODES_BUILTIN_HH

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace vfs
{
	
	node_ptr builtin_factory( const node*          parent,
	                          const plus::string&  name,
	                          const void*          args );
	
}

#endif

