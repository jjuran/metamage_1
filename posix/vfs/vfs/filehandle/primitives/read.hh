/*
	read.hh
	-------
*/

#ifndef VFS_FILEHANDLE_PRIMITIVES_READ_HH
#define VFS_FILEHANDLE_PRIMITIVES_READ_HH

// POSIX
#include <sys/types.h>

// vfs
#include "vfs/filehandle_fwd.hh"


namespace vfs
{
	
	ssize_t read( filehandle& that, char* buffer, size_t n );
	
}

#endif

