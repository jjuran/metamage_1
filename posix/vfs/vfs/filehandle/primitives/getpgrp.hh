/*
	getpgrp.hh
	----------
*/

#ifndef VFS_FILEHANDLE_PRIMITIVES_GETPGRP_HH
#define VFS_FILEHANDLE_PRIMITIVES_GETPGRP_HH

// POSIX
#include <sys/types.h>

// vfs
#include "vfs/filehandle_fwd.hh"


namespace vfs
{
	
	pid_t getpgrp( filehandle& that );
	
}

#endif

