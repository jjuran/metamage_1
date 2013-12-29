/*
	fstatat.cc
	----------
*/

#include "relix/syscall/fstatat.hh"

// POSIX
#include <sys/stat.h>

// Standard C/C++
#include <cstring>

// vfs
#include "vfs/node.hh"
#include "vfs/functions/resolve_links_in_place.hh"
#include "vfs/primitives/geteof.hh"
#include "vfs/primitives/stat.hh"

// relix
#include "relix/api/errno.hh"
#include "relix/fs/resolve_path_at.hh"


#ifndef AT_SYMLINK_NOFOLLOW
#define AT_SYMLINK_NOFOLLOW  0
#endif


namespace relix
{
	
	int fstatat( int dirfd, const char* path, struct stat* sb, int flags )
	{
		std::memset( (void*) sb, '\0', sizeof (struct stat) );
		
		sb->st_size = off_t( -1 );
		
		try
		{
			vfs::node_ptr file = resolve_path_at( dirfd, path );
			
			if ( const bool following_links = !(flags & AT_SYMLINK_NOFOLLOW) )
			{
				vfs::resolve_links_in_place( file );
			}
			
			stat( *file, *sb );
			
			if ( sb->st_size == off_t( -1 ) )
			{
				sb->st_size = geteof( *file );
			}
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
}

