/*
	open.hh
	-------
*/

#ifndef VFS_PRIMITIVES_OPEN_HH
#define VFS_PRIMITIVES_OPEN_HH

// POSIX
#include <sys/types.h>

// vfs
#include "vfs/filehandle_ptr.hh"
#include "vfs/node_fwd.hh"


namespace vfs
{
	
	filehandle_ptr open( const node* that, int flags, mode_t mode );
	
}

#endif

