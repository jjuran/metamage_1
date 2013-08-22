/*
	attach.hh
	---------
*/

#ifndef VFS_PRIMITIVES_ATTACH
#define VFS_PRIMITIVES_ATTACH

// vfs
#include "vfs/node_fwd.hh"


namespace vfs
{
	
	void attach( const node& that, const node& target );
	
}

#endif

