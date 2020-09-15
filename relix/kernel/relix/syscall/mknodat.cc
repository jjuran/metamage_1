/*
	mknodat.cc
	----------
*/

#include "relix/syscall/mknodat.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/primitives/mknod.hh"

// relix-kernel
#include "relix/api/errno.hh"
#include "relix/fs/resolve_path_at.hh"


namespace relix
{
	
	int mknodat( int dirfd, const char* path, mode_t mode, dev_t dev )
	{
		try
		{
			vfs::node_ptr location = resolve_path_at( dirfd, path );
			
			// Do not resolve links
			
			mknod( *location, mode, dev );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
}
