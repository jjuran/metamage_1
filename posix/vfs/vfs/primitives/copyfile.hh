/*
	copyfile.hh
	-----------
*/

#ifndef VFS_PRIMITIVES_COPYFILE
#define VFS_PRIMITIVES_COPYFILE

// vfs
#include "vfs/node_fwd.hh"


namespace vfs
{
	
	void copyfile( const node& that, const node& target );
	
}

#endif

