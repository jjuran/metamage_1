/*
	parent_inode.hh
	---------------
*/

#ifndef VFS_PRIMITIVES_PARENTINODE
#define VFS_PRIMITIVES_PARENTINODE

// POSIX
#include <sys/types.h>

// vfs
#include "vfs/node_fwd.hh"


namespace vfs
{
	
	ino_t parent_inode( const node* it );
	
}

#endif

