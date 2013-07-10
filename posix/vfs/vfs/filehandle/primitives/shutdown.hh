/*
	shutdown.hh
	-----------
*/

#ifndef VFS_FILEHANDLE_PRIMITIVES_SHUTDOWN_HH
#define VFS_FILEHANDLE_PRIMITIVES_SHUTDOWN_HH

// vfs
#include "vfs/filehandle_fwd.hh"


namespace vfs
{
	
	void shutdown( filehandle& that, int how );
	
}

#endif

