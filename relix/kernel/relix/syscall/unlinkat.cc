/*
	unlinkat.cc
	-----------
*/

#include "relix/syscall/unlinkat.hh"

// POSIX
#include <fcntl.h>

// Standard C
#include <errno.h>

// vfs
#include "vfs/node.hh"
#include "vfs/functions/file-tests.hh"
#include "vfs/primitives/remove.hh"

// relix
#include "relix/api/errno.hh"
#include "relix/fs/resolve_path_at.hh"


#ifndef AT_REMOVEANY
#define AT_REMOVEANY  0
#endif

#ifndef AT_REMOVEDIR
#define AT_REMOVEDIR  0
#endif


namespace relix
{
	
	int unlinkat( int dirfd, const char* path, int flags )
	{
		try
		{
			vfs::node_ptr file = resolve_path_at( dirfd, path );
			
			// Do not resolve links -- delete the symlink
			
			const bool remove_any = flags & AT_REMOVEANY;
			const bool remove_dir = flags & AT_REMOVEDIR;
			
			if ( remove_any || remove_dir == is_directory( *file ) )
			{
				remove( *file );
			}
			else
			{
				return set_errno( remove_dir ? ENOTDIR : EPERM );
			}
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
}

