/*
	fstatat.cc
	----------
*/

#include "relix/syscall/fstatat.hh"

// POSIX
#include <sys/stat.h>

// Standard C
#include <string.h>

// vfs
#include "vfs/node.hh"
#include "vfs/functions/resolve_links_in_place.hh"
#include "vfs/primitives/stat.hh"

// relix
#include "relix/api/errno.hh"
#include "relix/api/root.hh"
#include "relix/fs/resolve_path_at.hh"


#ifndef AT_SYMLINK_NOFOLLOW
#define AT_SYMLINK_NOFOLLOW  0
#endif


namespace relix
{
	
	int fstatat( int dirfd, const char* path, struct stat* sb, int flags )
	{
		memset( (void*) sb, '\0', sizeof (struct stat) );
		
		try
		{
			vfs::node_ptr file = resolve_path_at( dirfd, path );
			
			if ( const bool following_links = !(flags & AT_SYMLINK_NOFOLLOW) )
			{
				vfs::resolve_links_in_place( *root(), file );
			}
			
			stat( *file, *sb );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
}
