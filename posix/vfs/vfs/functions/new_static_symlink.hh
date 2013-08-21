/*
	new_static_symlink.hh
	---------------------
*/

#ifndef VFS_FUNCTIONS_STATICSYMLINK_HH
#define VFS_FUNCTIONS_STATICSYMLINK_HH

// plus
#include "plus/string_fwd.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace vfs
{
	
	node_ptr new_static_symlink( const node*          parent,
	                             const plus::string&  name,
	                             const void*          params );
	
}

#endif

