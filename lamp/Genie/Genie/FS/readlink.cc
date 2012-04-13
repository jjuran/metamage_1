/*
	readlink.cc
	-----------
*/

#include "Genie/FS/readlink.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/link_method_set.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/FS/pathname.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	plus::string readlink( const FSTree* it )
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

