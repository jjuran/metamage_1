/*
	geteof.cc
	---------
*/

#include "vfs/primitives/geteof.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/node_method_set.hh"


namespace vfs
{
	
	off_t geteof( const node& that )
	{
		const node_method_set* methods = that.methods();
		
		const data_method_set* data_methods;
		
		if ( methods  &&  (data_methods = methods->data_methods) )
		{
			if ( data_methods->geteof )
			{
				return data_methods->geteof( &that );
			}
		}
		
		// Errors are meaningless here since there's no POSIX call specifically
		// to get EOF (as there is truncate() for setting EOF).  We only care so
		// we can populate stat::st_size.  Just return zero.
		
		return 0;
	}
	
}

