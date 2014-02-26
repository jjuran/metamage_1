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
	
	node_ptr resolve( const node& root, const node& that )
	{
		const node_method_set* methods = that.methods();
		
		const link_method_set* link_methods;
		
		if ( methods  &&  (link_methods = methods->link_methods) )
		{
			if ( link_methods->resolve )
			{
				return link_methods->resolve( &that );
			}
			
			if ( link_methods->readlink )
			{
				return resolve_pathname( root, link_methods->readlink( &that ), *that.owner() );
			}
		}
		
		return &that;
	}
	
}

