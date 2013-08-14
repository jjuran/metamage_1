/*
	seteof.hh
	---------
*/

#ifndef VFS_FILEHANDLE_PRIMITIVES_SETEOF
#define VFS_FILEHANDLE_PRIMITIVES_SETEOF

// POSIX
#include <sys/types.h>

// vfs
#include "vfs/filehandle_fwd.hh"


namespace vfs
{
	
	void seteof( filehandle& that, off_t length );
	
}

#endif

