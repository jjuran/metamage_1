/*
	renameat.cc
	-----------
*/

#include "relix/syscall/renameat.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/primitives/rename.hh"

// relix
#include "relix/api/errno.hh"
#include "relix/fs/resolve_path_at.hh"


namespace relix
{
	
	int renameat( int olddirfd, const char* oldpath, int newdirfd, const char* newpath )
	{
		try
		{
			vfs::node_ptr src  = resolve_path_at( olddirfd, oldpath );
			vfs::node_ptr dest = resolve_path_at( newdirfd, newpath );
			
			// Do not resolve links
			
			rename( *src, *dest );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
}

