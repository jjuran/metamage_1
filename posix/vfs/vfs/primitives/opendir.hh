/*
	opendir.hh
	----------
*/

#ifndef VFS_PRIMITIVES_OPENDIR_HH
#define VFS_PRIMITIVES_OPENDIR_HH

// vfs
#include "vfs/filehandle_ptr.hh"
#include "vfs/node_fwd.hh"


namespace vfs
{
	
	filehandle_ptr opendir( const node* that );
	
}

#endif

