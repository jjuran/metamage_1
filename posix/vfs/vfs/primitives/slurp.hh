/*
	slurp.hh
	--------
*/

#ifndef VFS_PRIMITIVES_SLURP_HH
#define VFS_PRIMITIVES_SLURP_HH

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_fwd.hh"


namespace vfs
{
	
	plus::string slurp( const node* that );
	
}

#endif

