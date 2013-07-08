/*
	posix.hh
	--------
*/

#ifndef VFS_NODE_TYPES_POSIX_HH
#define VFS_NODE_TYPES_POSIX_HH

// plus
#include "plus/string_fwd.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace vfs
{
	
	node_ptr new_posix_root( const plus::string& path );
	
}

#endif

