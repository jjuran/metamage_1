/*	================
 *	ResolvePathAt.cc
 *	================
 */

#include "Genie/FS/ResolvePathAt.hh"

// vfs
#include "vfs/node.hh"

// relix
#include "relix/fs/resolve_path_at.hh"


namespace Genie
{
	
	vfs::node_ptr ResolvePathAt( int dirfd, const plus::string& path )
	{
		return relix::resolve_path_at( dirfd, path );
	}
	
}

