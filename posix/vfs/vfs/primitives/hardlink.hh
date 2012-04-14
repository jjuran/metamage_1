/*
	hardlink.hh
	-----------
*/

#ifndef VFS_PRIMITIVES_HARDLINK
#define VFS_PRIMITIVES_HARDLINK

// vfs
#include "vfs/node_fwd.hh"


namespace vfs
{
	
	void hardlink( const node* it, const node* target );
	
}

#endif

