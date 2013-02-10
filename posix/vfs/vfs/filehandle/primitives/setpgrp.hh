/*
	setpgrp.hh
	----------
*/

#ifndef VFS_FILEHANDLE_PRIMITIVES_SETPGRP_HH
#define VFS_FILEHANDLE_PRIMITIVES_SETPGRP_HH

// POSIX
#include <sys/types.h>

// vfs
#include "vfs/filehandle_fwd.hh"


namespace vfs
{
	
	void setpgrp( filehandle& that, pid_t pgid );
	
}

#endif

