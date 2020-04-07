/*
	bind.hh
	-------
*/

#ifndef VFS_FILEHANDLE_PRIMITIVES_BIND_HH
#define VFS_FILEHANDLE_PRIMITIVES_BIND_HH

// POSIX
#ifdef __APPLE__
// This is needed to compile with Mac OS X 10.3's headers.
#include <sys/types.h>
#endif
#include <sys/socket.h>

// vfs
#include "vfs/filehandle_fwd.hh"

#ifdef __APPLE__
#ifndef _SOCKLEN_T
typedef int socklen_t;
#endif
#endif


namespace vfs
{
	
	void bind( filehandle& that, const sockaddr& local, socklen_t len );
	
}

#endif
