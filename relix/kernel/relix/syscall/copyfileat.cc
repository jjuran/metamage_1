/*
	copyfileat.cc
	-------------
*/

#include "relix/syscall/copyfileat.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/primitives/copyfile.hh"
#include "vfs/functions/resolve_links_in_place.hh"
#include "vfs/functions/resolve_pathname.hh"

// relix
#include "relix/api/errno.hh"
#include "relix/fs/resolve_path_at.hh"


namespace relix
{
	
	int copyfileat( int olddirfd, const char* oldpath, int newdirfd, const char* newpath, unsigned flags )
	{
		try
		{
			vfs::node_ptr src  = resolve_path_at( olddirfd, oldpath );
			vfs::node_ptr dest = resolve_path_at( newdirfd, newpath );
			
			// Do not resolve links
			
			copyfile( *src, *dest );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
}

