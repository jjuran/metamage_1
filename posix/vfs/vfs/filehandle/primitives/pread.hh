/*
	pread.hh
	--------
*/

#ifndef VFS_FILEHANDLE_PRIMITIVES_PREAD_HH
#define VFS_FILEHANDLE_PRIMITIVES_PREAD_HH

// POSIX
#include <sys/types.h>

// vfs
#include "vfs/filehandle_fwd.hh"


namespace vfs
{
	
	ssize_t pread( filehandle& that, char* buffer, size_t n, off_t offset );
	
}

#endif

