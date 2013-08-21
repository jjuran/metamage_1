/*	==================
 *	ResolvePathname.cc
 *	==================
 */

#include "Genie/FS/ResolvePathname.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/primitives/resolve.hh"

// Genie
#include "Genie/FS/file-tests.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	bool ResolveLinks_InPlace( FSTreePtr& file )
	{
		unsigned link_count = 0;
		
		while ( is_symlink( file ) )
		{
			++link_count;
			
			if ( link_count > 10 )
			{
				p7::throw_errno( ELOOP );
			}
			
			file = resolve( file.get() );
		}
		
		return link_count > 0;
	}
	
}

