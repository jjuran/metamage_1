/*
	pathname.hh
	-----------
*/

#ifndef VFS_FUNCTIONS_PATHNAME
#define VFS_FUNCTIONS_PATHNAME

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_fwd.hh"


namespace vfs
{
	
	plus::string pathname( const node& that );
	
}

#endif

