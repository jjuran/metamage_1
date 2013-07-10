/*
	listen.hh
	---------
*/

#ifndef VFS_FILEHANDLE_PRIMITIVES_LISTEN_HH
#define VFS_FILEHANDLE_PRIMITIVES_LISTEN_HH

// vfs
#include "vfs/filehandle_fwd.hh"


namespace vfs
{
	
	void listen( filehandle& that, int backlog );
	
}

#endif

