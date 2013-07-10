/*
	bind.hh
	-------
*/

#ifndef VFS_FILEHANDLE_PRIMITIVES_BIND_HH
#define VFS_FILEHANDLE_PRIMITIVES_BIND_HH

// POSIX
#include <sys/socket.h>

// vfs
#include "vfs/filehandle_fwd.hh"


namespace vfs
{
	
	void bind( filehandle& that, const sockaddr& local, socklen_t len );
	
}

#endif

