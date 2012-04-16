/*
	lookup.hh
	---------
*/

#ifndef VFS_PRIMITIVES_LOOKUP
#define VFS_PRIMITIVES_LOOKUP

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace vfs
{
	
	node_ptr lookup( const node*          it,
	                 const plus::string&  name,
	                 const node*          surrogate = NULL );
	
}

#endif

