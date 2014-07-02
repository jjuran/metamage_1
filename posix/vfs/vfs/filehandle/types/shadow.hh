/*
	shadow.hh
	---------
*/

#ifndef VFS_FILEHANDLE_TYPES_SHADOW_HH
#define VFS_FILEHANDLE_TYPES_SHADOW_HH

// vfs
#include "vfs/filehandle_ptr.hh"
#include "vfs/node_fwd.hh"


namespace vfs
{
	
	filehandle_ptr new_shadow( const node& file, int flags, filehandle& basis );
	
}

#endif
