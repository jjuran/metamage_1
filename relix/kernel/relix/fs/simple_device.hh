/*
	simple_device.hh
	----------------
*/

#ifndef RELIX_FS_SIMPLEDEVICE_HH
#define RELIX_FS_SIMPLEDEVICE_HH

// vfs
#include "vfs/filehandle_ptr.hh"
#include "vfs/node_fwd.hh"


namespace relix
{
	
	vfs::filehandle_ptr open_simple_device( const vfs::node& file );
	
}

#endif
