/*
	fifo.hh
	-------
*/

#ifndef RELIX_FS_FIFO_HH
#define RELIX_FS_FIFO_HH

// vfs
#include "vfs/filehandle_ptr.hh"
#include "vfs/node_fwd.hh"


namespace relix
{
	
	vfs::filehandle_ptr open_fifo( const vfs::node*  that,
	                               int               flags );
	
}

#endif
