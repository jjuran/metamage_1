/*
	resolve.cc
	----------
*/

#include "vfs/primitives/resolve.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/methods/link_method_set.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/node_ptr.hh"
#include "vfs/functions/resolve_pathname.hh"


namespace vfs
{
	
	node_ptr resolve( const node* it )
	{
		const node_method_set* methods = it->methods();
		
		const link_method_set* link_methods;
		
		if ( methods  &&  (link_methods = methods->link_methods) )
		{
			if ( link_methods->resolve )
			{
				return link_methods->resolve( it );
			}
			
			if ( link_methods->readlink )
			{
				return resolve_pathname( link_methods->readlink( it ), it->owner() );
			}
		}
		
		return it;
	}
	
}

