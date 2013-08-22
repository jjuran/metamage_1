/*
	mkdir.hh
	--------
*/

#ifndef VFS_PRIMITIVES_MKDIR
#define VFS_PRIMITIVES_MKDIR

// POSIX
#include <sys/types.h>

// vfs
#include "vfs/node_fwd.hh"


namespace vfs
{
	
	void mkdir( const node& that, mode_t mode );
	
}

#endif

