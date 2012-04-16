/*
	chmod.hh
	--------
*/

#ifndef VFS_PRIMITIVES_CHMOD
#define VFS_PRIMITIVES_CHMOD

// POSIX
#include <sys/types.h>

// vfs
#include "vfs/node_fwd.hh"


namespace vfs
{
	
	void chmod( const node* it, mode_t mode );
	
}

#endif

