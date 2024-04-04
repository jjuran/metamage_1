/*
	faccessat.cc
	------------
*/

#include "relix/syscall/faccessat.hh"

// POSIX
#include <sys/stat.h>

// Standard C
#include <errno.h>

// vfs
#include "vfs/node.hh"
#include "vfs/functions/file-tests.hh"
#include "vfs/functions/resolve_links_in_place.hh"

// relix
#include "relix/api/errno.hh"
#include "relix/api/root.hh"
#include "relix/fs/resolve_path_at.hh"


#ifndef AT_SYMLINK_NOFOLLOW
#define AT_SYMLINK_NOFOLLOW  0
#endif


namespace relix
{
	
	int faccessat( int dirfd, const char* path, mode_t mode, int flags )
	{
		try
		{
			vfs::node_ptr file = resolve_path_at( dirfd, path );
			
			if ( const bool following_links = !(flags & AT_SYMLINK_NOFOLLOW) )
			{
				vfs::resolve_links_in_place( *root(), file );
			}
			
			if ( !exists( *file ) )
			{
				return set_errno( ENOENT );
			}
			
			// FIXME: check permissions
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
}
