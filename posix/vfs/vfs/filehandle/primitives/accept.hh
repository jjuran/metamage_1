/*
	accept.hh
	---------
*/

#ifndef VFS_FILEHANDLE_PRIMITIVES_ACCEPT_HH
#define VFS_FILEHANDLE_PRIMITIVES_ACCEPT_HH

// POSIX
#ifdef __APPLE__
// This is needed to compile with Mac OS X 10.3's headers.
#include <sys/types.h>
#endif
#include <sys/socket.h>

// vfs
#include "vfs/filehandle_ptr.hh"

#ifdef __APPLE__
#ifndef _SOCKLEN_T
typedef int socklen_t;
#endif
#endif


namespace vfs
{
	
	filehandle_ptr accept( filehandle& that, sockaddr& local, socklen_t& len );
	
}

#endif
