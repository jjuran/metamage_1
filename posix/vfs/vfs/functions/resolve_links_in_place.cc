/*
	resolve_links_in_place.cc
	-------------------------
*/

#include "vfs/functions/resolve_links_in_place.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/functions/file-tests.hh"
#include "vfs/primitives/resolve.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	void resolve_links_in_place( const node& root, node_ptr& file )
	{
		unsigned link_count = 0;
		
		while ( is_symlink( *file ) )
		{
			++link_count;
			
			if ( link_count > 10 )
			{
				p7::throw_errno( ELOOP );
			}
			
			file = resolve( root, *file );
		}
	}
	
}

