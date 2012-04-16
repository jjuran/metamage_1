/*
	inode.hh
	--------
*/

#ifndef VFS_PRIMITIVES_INODE
#define VFS_PRIMITIVES_INODE

// POSIX
#include <sys/types.h>

// vfs
#include "vfs/node_fwd.hh"


namespace vfs
{
	
	ino_t inode( const node* it );
	
}

#endif

