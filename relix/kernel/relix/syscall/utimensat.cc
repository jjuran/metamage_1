/*
	utimensat.cc
	------------
*/

#include "relix/syscall/utimensat.hh"

// POSIX
#include <time.h>
#include <sys/stat.h>

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/functions/resolve_links_in_place.hh"
#include "vfs/primitives/touch.hh"
#include "vfs/primitives/utime.hh"

// relix
#include "relix/api/errno.hh"
#include "relix/api/get_fd_handle.hh"
#include "relix/api/root.hh"
#include "relix/fs/resolve_path_at.hh"


#ifndef AT_SYMLINK_NOFOLLOW
#define AT_SYMLINK_NOFOLLOW  0
#endif


namespace relix
{
	
	static inline bool merely_touch( const timespec* t )
	{
		if ( t != NULL )
		{
		#ifdef UTIME_NOW
			
			if ( t[0].tv_nsec == UTIME_NOW  &&  t[1].tv_nsec == UTIME_NOW )
			{
				return true;
			}
			
		#endif
		}
		
		return t == NULL;
	}
	
	int utimensat( int fd, const char* path, const timespec* times, int flags )
	{
		try
		{
			vfs::node_ptr file = path != NULL ? resolve_path_at( fd, path )
			                                  : get_fd_handle( fd ).GetFile();
			
			const bool nofollow = flags & AT_SYMLINK_NOFOLLOW;
			
			if ( !nofollow  &&  path != NULL )
			{
				vfs::resolve_links_in_place( *root(), file );
			}
			
			if ( merely_touch( times ) )
			{
				touch( *file );
			}
			else
			{
				utime( *file, times );
			}
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
}

