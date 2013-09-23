/*
	posix.hh
	--------
*/

#ifndef VFS_FILEHANDLE_TYPES_POSIX_HH
#define VFS_FILEHANDLE_TYPES_POSIX_HH

// vfs
#include "vfs/filehandle_ptr.hh"


namespace vfs
{
	
	filehandle_ptr new_posix_fd( int flags, int fd );
	
}

#endif

