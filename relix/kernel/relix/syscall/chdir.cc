/*
	chdir.cc
	--------
*/

#include "relix/syscall/chdir.hh"

// Standard C
#include <errno.h>

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/node_ptr.hh"
#include "vfs/functions/file-tests.hh"
#include "vfs/functions/resolve_links_in_place.hh"
#include "vfs/functions/resolve_pathname.hh"
#include "vfs/primitives/opendir.hh"

// relix-kernel
#include "relix/api/current_process.hh"
#include "relix/api/errno.hh"
#include "relix/api/getcwd.hh"
#include "relix/api/root.hh"
#include "relix/task/fs_info.hh"
#include "relix/task/process.hh"
#include "relix/task/process_resources.hh"


namespace relix
{
	
	int chdir( const char* pathname )
	{
		try
		{
			if ( pathname == NULL )
			{
				return set_errno( EINVAL );
			}
			
			vfs::node_ptr new_cwd = resolve_pathname( *relix::root(), pathname, *getcwd() );
			
			vfs::resolve_links_in_place( *relix::root(), new_cwd );
			
			if ( !is_directory( *new_cwd ) )
			{
				return set_errno( exists( *new_cwd ) ? ENOTDIR : ENOENT );
			}
			
			current_process().get_process_resources().get_fs_info().chdir( opendir( *new_cwd ) );
			
			return 0;
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
	}
	
}

