/*
	access.hh
	---------
*/

#ifndef VFS_FUNCTIONS_ACCESS
#define VFS_FUNCTIONS_ACCESS

// vfs
#include "vfs/node_fwd.hh"


namespace vfs
{
	
	void access( const node& that, int amode, int flags = 0 );
	
}

#endif
