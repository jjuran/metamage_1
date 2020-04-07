/*
	stat.hh
	-------
*/

#ifndef VFS_PRIMITIVES_STAT
#define VFS_PRIMITIVES_STAT

// vfs
#include "vfs/node_fwd.hh"


// <sys/stat.h>
struct stat;


namespace vfs
{
	
	void stat( const node& that, struct stat& sb );
	
}

#endif
