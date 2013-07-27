/*
	slurp.cc
	--------
*/

#include "vfs/primitives/slurp.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/node_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	plus::string slurp( const node* that )
	{
		const node_method_set* methods = that->methods();
		
		const data_method_set* data_methods;
		
		if ( methods  &&  (data_methods = methods->data_methods) )
		{
			if ( data_methods->slurp )
			{
				return data_methods->slurp( that );
			}
			
			// FIXME:  Try open/read
		}
		
		throw p7::errno_t( EINVAL );
	}
	
}

