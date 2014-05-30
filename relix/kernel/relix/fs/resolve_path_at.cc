/*
	resolve_path_at.cc
	------------------
*/

#include "relix/fs/resolve_path_at.hh"

// POSIX
#include <fcntl.h>

// Standard C
#include <string.h>

// plus
#include "plus/string.hh"

// POSeven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/filehandle/primitives/get_file.hh"
#include "vfs/functions/resolve_pathname.hh"

// relix
#include "relix/api/getcwd.hh"
#include "relix/api/get_fd_handle.hh"
#include "relix/api/root.hh"


#ifndef AT_FDCWD
#define AT_FDCWD  (-100)
#endif


namespace relix
{
	
	namespace p7 = poseven;
	
	
	vfs::node_ptr resolve_path_at( int dirfd, const char* path, unsigned length )
	{
		if ( length == 0 )
		{
			p7::throw_errno( ENOENT );
		}
		
		if ( path[0] == '/' )
		{
			return vfs::resolve_absolute_path( *root(), path, length );
		}
		
		vfs::node_ptr at_dir = dirfd == AT_FDCWD ? getcwd()
		                                         : get_file( get_fd_handle( dirfd ) );
		
		return resolve_pathname( *root(), path, length, *at_dir );
	}
	
	vfs::node_ptr resolve_path_at( int dirfd, const plus::string& path )
	{
		return resolve_path_at( dirfd, path.data(), path.size() );
	}
	
	vfs::node_ptr resolve_path_at( int dirfd, const char* path )
	{
		return resolve_path_at( dirfd, path, strlen( path ) );
	}
	
}

