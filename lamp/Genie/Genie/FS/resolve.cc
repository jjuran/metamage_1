/*
	resolve.cc
	----------
*/

#include "Genie/FS/resolve.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/ResolvePathname.hh"
#include "Genie/FS/link_method_set.hh"
#include "Genie/FS/node_method_set.hh"


namespace Genie
{
	
	FSTreePtr resolve( const FSTree* node )
	{
		const node_method_set* methods = node->methods();
		
		const link_method_set* link_methods;
		
		if ( methods  &&  (link_methods = methods->link_methods) )
		{
			if ( link_methods->resolve )
			{
				return link_methods->resolve( node );
			}
			
			if ( link_methods->readlink )
			{
				return ResolvePathname( link_methods->readlink( node ), node->owner() );
			}
		}
		
		return node;
	}
	
}

