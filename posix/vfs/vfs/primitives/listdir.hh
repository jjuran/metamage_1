/*
	listdir.hh
	----------
*/

#ifndef VFS_PRIMITIVES_LISTDIR
#define VFS_PRIMITIVES_LISTDIR

// vfs
#include "vfs/dir_contents_fwd.hh"
#include "vfs/node_fwd.hh"


namespace vfs
{
	
	void listdir( const node* it, vfs::dir_contents& contents );
	
}

#endif

