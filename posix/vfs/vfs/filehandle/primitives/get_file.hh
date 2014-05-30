/*
	get_file.hh
	-----------
*/

#ifndef VFS_FILEHANDLE_PRIMITIVES_GETFILE_HH
#define VFS_FILEHANDLE_PRIMITIVES_GETFILE_HH

// vfs
#include "vfs/filehandle_fwd.hh"
#include "vfs/node_ptr.hh"


namespace vfs
{
	
	node_ptr get_file( filehandle& that );
	
}

#endif
