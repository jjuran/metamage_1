/*
	mknod.hh
	--------
*/

#ifndef VFS_PRIMITIVES_MKNOD
#define VFS_PRIMITIVES_MKNOD

// POSIX
#include <sys/types.h>

// vfs
#include "vfs/node_fwd.hh"


namespace vfs
{
	
	void mknod( const node& that, mode_t mode, dev_t dev );
	
}

#endif
