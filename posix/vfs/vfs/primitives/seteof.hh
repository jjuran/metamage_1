/*
	seteof.hh
	---------
*/

#ifndef VFS_PRIMITIVES_SETEOF
#define VFS_PRIMITIVES_SETEOF

// POSIX
#include <sys/types.h>

// vfs
#include "vfs/node_fwd.hh"


namespace vfs
{
	
	void seteof( const node* it, off_t length );
	
}

#endif

