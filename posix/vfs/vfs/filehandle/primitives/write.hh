/*
	write.hh
	--------
*/

#ifndef VFS_FILEHANDLE_PRIMITIVES_WRITE_HH
#define VFS_FILEHANDLE_PRIMITIVES_WRITE_HH

// POSIX
#include <sys/types.h>

// vfs
#include "vfs/filehandle_fwd.hh"


namespace vfs
{
	
	ssize_t write( filehandle& that, const char* buffer, size_t n );
	
}

#endif

