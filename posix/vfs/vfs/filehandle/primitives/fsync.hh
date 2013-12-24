/*
	fsync.hh
	--------
*/

#ifndef VFS_FILEHANDLE_PRIMITIVES_FSYNC
#define VFS_FILEHANDLE_PRIMITIVES_FSYNC

// vfs
#include "vfs/filehandle_fwd.hh"


namespace vfs
{
	
	void fsync( filehandle& that );
	
}

#endif

