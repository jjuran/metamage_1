/*
	geteof.hh
	---------
*/

#ifndef VFS_FILEHANDLE_PRIMITIVES_GETEOF
#define VFS_FILEHANDLE_PRIMITIVES_GETEOF

// POSIX
#include <sys/types.h>

// vfs
#include "vfs/filehandle_fwd.hh"


namespace vfs
{
	
	off_t geteof( filehandle& that );
	
}

#endif

