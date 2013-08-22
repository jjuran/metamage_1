/*
	utime.hh
	--------
*/

#ifndef VFS_PRIMITIVES_UTIME
#define VFS_PRIMITIVES_UTIME

// vfs
#include "vfs/node_fwd.hh"


// time.h
struct timespec;


namespace vfs
{
	
	void utime( const node& that, const struct timespec* times );
	
}

#endif

