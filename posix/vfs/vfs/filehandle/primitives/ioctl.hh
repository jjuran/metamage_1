/*
	ioctl.hh
	---------
*/

#ifndef VFS_FILEHANDLE_PRIMITIVES_IOCTL_HH
#define VFS_FILEHANDLE_PRIMITIVES_IOCTL_HH

// vfs
#include "vfs/filehandle_fwd.hh"


namespace vfs
{
	
	void ioctl( filehandle* that, unsigned long request, int* argp );
	
}

#endif

