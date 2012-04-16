/*
	symlink.hh
	----------
*/

#ifndef VFS_PRIMITIVES_SYMLINK
#define VFS_PRIMITIVES_SYMLINK

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_fwd.hh"


namespace vfs
{
	
	void symlink( const node* it, const plus::string& target );
	
}

#endif

