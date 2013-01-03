/*
	anonymous.hh
	------------
*/

#ifndef VFS_NODES_ANONYMOUS_HH
#define VFS_NODES_ANONYMOUS_HH

// POSIX
#include <sys/types.h>

// plus
#include "plus/string_fwd.hh"

// vfs
#include "vfs/filehandle_fwd.hh"
#include "vfs/node_ptr.hh"


namespace vfs
{
	
	node_ptr new_anonymous_node( const plus::string&  name,
	                             mode_t               mode,
	                             filehandle*          h );
	
}

#endif

