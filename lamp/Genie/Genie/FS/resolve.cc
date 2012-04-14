/*
	resolve.cc
	----------
*/

#include "Genie/FS/resolve.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/methods/link_method_set.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/node_ptr.hh"

// Genie
#include "Genie/FS/ResolvePathname.hh"


namespace vfs
{
	
	using Genie::ResolvePathname;
	
	
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
				return ResolvePathname( link_methods->readlink( it ), it->owner() );
			}
		}
		
		return it;
	}
	
}

