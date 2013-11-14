/*
	truncate.cc
	-----------
*/

#include "relix/syscall/truncate.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/primitives/seteof.hh"
#include "vfs/functions/resolve_links_in_place.hh"
#include "vfs/functions/resolve_pathname.hh"

// relix
#include "relix/api/errno.hh"
#include "relix/api/getcwd.hh"


namespace relix
{
	
	int truncate( const char* path, off_t length )
	{
		try
		{
			vfs::node_ptr file = resolve_pathname( path, *getcwd() );
			
			vfs::resolve_links_in_place( file );
			
			seteof( *file, length );
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
		
		return 0;
	}
	
}

