/*
	readlink.cc
	-----------
*/

#include "vfs/primitives/readlink.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/methods/link_method_set.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/functions/pathname.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	plus::string readlink( const node* it )
	{
		const node_method_set* methods = it->methods();
		
		const link_method_set* link_methods;
		
		if ( methods  &&  (link_methods = methods->link_methods) )
		{
			if ( link_methods->readlink )
			{
				return link_methods->readlink( it );
			}
			
			if ( link_methods->resolve )
			{
				return pathname( link_methods->resolve( it ).get() );
			}
		}
		
		throw p7::errno_t( EINVAL );
	}
	
}

