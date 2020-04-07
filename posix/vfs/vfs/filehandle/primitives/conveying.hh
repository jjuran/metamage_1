/*
	conveying.hh
	------------
*/

#ifndef VFS_FILEHANDLE_PRIMITIVES_CONVEYING_HH
#define VFS_FILEHANDLE_PRIMITIVES_CONVEYING_HH

// POSIX
#ifdef __APPLE__
// This is needed to compile with Mac OS X 10.3's headers.
#include <sys/types.h>
#endif
#include <sys/socket.h>

// vfs
#include "vfs/filehandle_fwd.hh"


namespace vfs
{
	
	int conveying( filehandle& that );
	
}

#endif
