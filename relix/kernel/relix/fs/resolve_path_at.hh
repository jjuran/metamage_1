/*
	resolve_path_at.hh
	------------------
*/

#ifndef RELIX_FS_RESOLVEPATHAT_HH
#define RELIX_FS_RESOLVEPATHAT_HH

// plus
#include "plus/string_fwd.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace relix
{
	
	vfs::node_ptr resolve_path_at( int dirfd, const plus::string& path );
	
}

#endif

