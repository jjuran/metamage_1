/*
	mac.hh
	------
*/

#ifndef VFS_NODE_TYPES_MAC_HH
#define VFS_NODE_TYPES_MAC_HH

// POSIX
#include <sys/types.h>

// vfs
#include "vfs/node_ptr.hh"


namespace vfs
{
	
	node_ptr mac_lookup_info( const char*  path,
	                          const char*  name,
	                          const node*  parent,
	                          uid_t        user );
	
}

#endif
