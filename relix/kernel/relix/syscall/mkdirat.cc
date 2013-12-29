/*
	mkdirat.cc
	----------
*/

#include "relix/syscall/mkdirat.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/primitives/mkdir.hh"

// relix-kernel
#include "relix/api/errno.hh"
#include "relix/fs/resolve_path_at.hh"


namespace relix
{
	
	int mkdirat( int dirfd, const char* path, mode_t mode )
	{
		try
		{
			vfs::node_ptr location = resolve_path_at( dirfd, path );
			
			// Do not resolve links
			
			mkdir( *location, mode );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
}

