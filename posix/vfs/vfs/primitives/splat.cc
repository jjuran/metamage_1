/*
	splat.cc
	--------
*/

#include "vfs/primitives/splat.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/node_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	void splat( const node& that, const char* data, size_t size )
	{
		const node_method_set* methods = that.methods();
		
		const data_method_set* data_methods;
		
		if ( methods  &&  (data_methods = methods->data_methods) )
		{
			if ( data_methods->splat )
			{
				data_methods->splat( &that, data, size );
				return;
			}
			
			// FIXME:  Try open/write
		}
		
		throw p7::errno_t( EINVAL );
	}
	
}
