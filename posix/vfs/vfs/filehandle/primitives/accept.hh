/*
	accept.hh
	---------
*/

#ifndef VFS_FILEHANDLE_PRIMITIVES_ACCEPT_HH
#define VFS_FILEHANDLE_PRIMITIVES_ACCEPT_HH

// POSIX
#include <sys/socket.h>

// vfs
#include "vfs/filehandle_ptr.hh"


namespace vfs
{
	
	filehandle_ptr accept( filehandle& that, sockaddr& local, socklen_t& len );
	
}

#endif

