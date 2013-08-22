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
	
	
	plus::string readlink( const node& that)
	{
		const node_method_set* methods = that.methods();
		
		const link_method_set* link_methods;
		
		if ( methods  &&  (link_methods = methods->link_methods) )
		{
			if ( link_methods->readlink )
			{
				return link_methods->readlink( &that );
			}
			
			if ( link_methods->resolve )
			{
				return pathname( *link_methods->resolve( &that ) );
			}
		}
		
		throw p7::errno_t( EINVAL );
	}
	
}

