/*
	ax2.hh
	------
*/

#ifndef VFS_FILEHANDLE_TYPES_AX2_HH
#define VFS_FILEHANDLE_TYPES_AX2_HH

// vfs
#include "vfs/filehandle_ptr.hh"


namespace vfs
{
	
	filehandle_ptr new_ax2_rsrc_access( int flags, int fd );
	
}

#endif
