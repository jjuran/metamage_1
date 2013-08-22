/*
	symlink.cc
	----------
*/

#include "vfs/primitives/symlink.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/methods/link_method_set.hh"
#include "vfs/methods/node_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	void symlink( const node& that, const plus::string& target )
	{
		const node_method_set* methods = that.methods();
		
		const link_method_set* link_methods;
		
		if ( methods  &&  (link_methods = methods->link_methods) )
		{
			if ( link_methods->symlink )
			{
				link_methods->symlink( &that, target );
				
				return;
			}
		}
		
		p7::throw_errno( EINVAL );
	}
	
}

