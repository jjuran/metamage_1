/*
	resolve_path_at.cc
	------------------
*/

#include "relix/fs/resolve_path_at.hh"

// POSIX
#include <fcntl.h>

// plus
#include "plus/string.hh"

// POSeven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/functions/resolve_pathname.hh"

// relix
#include "relix/api/getcwd.hh"
#include "relix/api/get_fd_handle.hh"


#ifndef AT_FDCWD
#define AT_FDCWD  (-100)
#endif


namespace relix
{
	
	namespace p7 = poseven;
	
	
	vfs::node_ptr resolve_path_at( int dirfd, const plus::string& path )
	{
		if ( path.empty() )
		{
			p7::throw_errno( ENOENT );
		}
		
		if ( path[0] == '/' )
		{
			return vfs::resolve_absolute_path( path );
		}
		
		vfs::node_ptr at_dir = dirfd == AT_FDCWD ? getcwd()
		                                         : get_fd_handle( dirfd ).GetFile();
		
		return resolve_pathname( path, *at_dir );
	}
	
}

