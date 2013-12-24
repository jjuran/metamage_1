/*
	gestalt.hh
	----------
*/

#ifndef MACVFS_FILE_GESTALT_HH
#define MACVFS_FILE_GESTALT_HH

// vfs
#include "vfs/filehandle_ptr.hh"
#include "vfs/node_fwd.hh"


namespace vfs
{
	
	filehandle_ptr open_gestalt( const node* that, int flags, mode_t mode );
	
}

#endif

