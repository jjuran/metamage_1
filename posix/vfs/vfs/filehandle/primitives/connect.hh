/*
	connect.hh
	----------
*/

#ifndef VFS_FILEHANDLE_PRIMITIVES_CONNECT_HH
#define VFS_FILEHANDLE_PRIMITIVES_CONNECT_HH

// POSIX
#include <sys/socket.h>

// vfs
#include "vfs/filehandle_fwd.hh"


namespace vfs
{
	
	void connect( filehandle& that, const sockaddr& server, socklen_t len );
	
}

#endif

