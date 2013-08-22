/*
	geteof.hh
	---------
*/

#ifndef VFS_PRIMITIVES_GETEOF
#define VFS_PRIMITIVES_GETEOF

// POSIX
#include <sys/types.h>

// vfs
#include "vfs/node_fwd.hh"


namespace vfs
{
	
	off_t geteof( const node& that );
	
}

#endif

