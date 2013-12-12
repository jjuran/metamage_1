/*
	conveying.hh
	------------
*/

#ifndef VFS_FILEHANDLE_PRIMITIVES_CONVEYING_HH
#define VFS_FILEHANDLE_PRIMITIVES_CONVEYING_HH

// POSIX
#include <sys/socket.h>

// vfs
#include "vfs/filehandle_fwd.hh"


namespace vfs
{
	
	int conveying( filehandle& that );
	
}

#endif

