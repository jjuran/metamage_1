/*
	mac.hh
	------
*/

#ifndef VFS_NODE_TYPES_MAC_HH
#define VFS_NODE_TYPES_MAC_HH

// POSIX
#include <sys/types.h>

// plus
#include "plus/string_fwd.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace vfs
{
	
	node_ptr mac_lookup_info( const plus::string&  path,
	                          const plus::string&  name,
	                          const node*          parent,
	                          uid_t                user );
	
}

#endif
