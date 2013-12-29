/*
	symlinkat.cc
	------------
*/

#include "relix/syscall/symlinkat.hh"

// Standard C
#include <errno.h>

// vfs
#include "vfs/node.hh"
#include "vfs/functions/file-tests.hh"
#include "vfs/primitives/symlink.hh"

// relix
#include "relix/api/errno.hh"
#include "relix/fs/resolve_path_at.hh"


namespace relix
{
	
	int symlinkat( const char* target_path, int newdirfd, const char* newpath )
	{
		try
		{
			vfs::node_ptr link = resolve_path_at( newdirfd, newpath );
			
			// Do not resolve links.  If there's a symlink in this location, throw EEXIST.
			
			if ( exists( *link ) )
			{
				return set_errno( EEXIST );
			}
			
			symlink( *link, target_path );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
}

