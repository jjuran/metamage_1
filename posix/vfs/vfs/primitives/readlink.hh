/*
	readlink.hh
	-----------
*/

#ifndef VFS_PRIMITIVES_READLINK
#define VFS_PRIMITIVES_READLINK

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_fwd.hh"


namespace vfs
{
	
	plus::string readlink( const node& that);
	
}

#endif

