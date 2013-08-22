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
	
	void hardlink( const node& that, const node& target );
	
}

#endif

