/*
	getpeername.hh
	--------------
*/

#ifndef VFS_FILEHANDLE_PRIMITIVES_GETPEERNAME_HH
#define VFS_FILEHANDLE_PRIMITIVES_GETPEERNAME_HH

// vfs
#include "vfs/filehandle_fwd.hh"


struct sockaddr;

namespace vfs
{
	
	const sockaddr& getpeername( filehandle& that );
	
}

#endif

