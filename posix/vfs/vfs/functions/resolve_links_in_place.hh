/*
	resolve_links_in_place.hh
	-------------------------
*/

#ifndef VFS_FUNCTIONS_RESOLVELINKSINPLACE_HH
#define VFS_FUNCTIONS_RESOLVELINKSINPLACE_HH

// vfs
#include "vfs/node_ptr.hh"


namespace vfs
{
	
	void resolve_links_in_place( const node& root, node_ptr& file );
	
}

#endif

