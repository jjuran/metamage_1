/*
	fchmodat.cc
	-----------
*/

#include "relix/syscall/fchmodat.hh"

// POSIX
#include <sys/stat.h>

// vfs
#include "vfs/node.hh"
#include "vfs/functions/resolve_links_in_place.hh"
#include "vfs/primitives/chmod.hh"

// relix
#include "relix/api/errno.hh"
#include "relix/fs/resolve_path_at.hh"


#ifndef AT_SYMLINK_NOFOLLOW
#define AT_SYMLINK_NOFOLLOW  0
#endif


namespace relix
{
	
	int fchmodat( int dirfd, const char* path, mode_t mode, int flags )
	{
		try
		{
			vfs::node_ptr file = resolve_path_at( dirfd, path );
			
			if ( const bool following_links = !(flags & AT_SYMLINK_NOFOLLOW) )
			{
				vfs::resolve_links_in_place( file );
			}
			
			chmod( *file, mode );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
}

