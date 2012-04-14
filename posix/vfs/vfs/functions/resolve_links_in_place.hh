/*
	resolve_links_in_place.hh
	-------------------------
*/

#ifndef VFS_FUNCTIONS_RESOLVELINKSINPLACE_HH
#define VFS_FUNCTIONS_RESOLVELINKSINPLACE_HH

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace vfs
{
	
	void resolve_links_in_place( node_ptr& file );
	
}

#endif

