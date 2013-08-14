/*
	pwrite.hh
	---------
*/

#ifndef VFS_FILEHANDLE_PRIMITIVES_PWRITE_HH
#define VFS_FILEHANDLE_PRIMITIVES_PWRITE_HH

// POSIX
#include <sys/types.h>

// vfs
#include "vfs/filehandle_fwd.hh"


namespace vfs
{
	
	ssize_t pwrite( filehandle& that, const char* buffer, size_t n, off_t offset );
	
}

#endif

