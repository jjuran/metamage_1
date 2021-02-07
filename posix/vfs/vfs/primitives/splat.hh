/*
	splat.hh
	--------
*/

#ifndef VFS_PRIMITIVES_SPLAT_HH
#define VFS_PRIMITIVES_SPLAT_HH

// POSIX
#include <sys/types.h>

// vfs
#include "vfs/node_fwd.hh"


namespace vfs
{
	
	void splat( const node& that, const char* data, size_t size );
	
}

#endif
