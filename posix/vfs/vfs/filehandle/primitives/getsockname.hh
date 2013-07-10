/*
	getsockname.hh
	--------------
*/

#ifndef VFS_FILEHANDLE_PRIMITIVES_GETSOCKNAME_HH
#define VFS_FILEHANDLE_PRIMITIVES_GETSOCKNAME_HH

// vfs
#include "vfs/filehandle_fwd.hh"


struct sockaddr;

namespace vfs
{
	
	const sockaddr& getsockname( filehandle& that );
	
}

#endif

