/*
	seek.hh
	-------
*/

#ifndef VFS_FILEHANDLE_FUNCTIONS_SEEK_HH
#define VFS_FILEHANDLE_FUNCTIONS_SEEK_HH

// POSIX
#include <sys/types.h>

// vfs
#include "vfs/filehandle_fwd.hh"


namespace vfs
{
	
	off_t seek( filehandle& that, off_t offset, int whence );
	
}

#endif

