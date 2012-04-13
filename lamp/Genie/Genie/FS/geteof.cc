/*
	geteof.cc
	---------
*/

#include "Genie/FS/geteof.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/data_method_set.hh"
#include "Genie/FS/node_method_set.hh"


namespace Genie
{
	
	off_t geteof( const FSTree* it )
	{
		const node_method_set* methods = it->methods();
		
		const data_method_set* data_methods;
		
		if ( methods  &&  (data_methods = methods->data_methods) )
		{
			if ( data_methods->geteof )
			{
				return data_methods->geteof( it );
			}
		}
		
		// Errors are meaningless here since there's no POSIX call specifically
		// to get EOF (as there is truncate() for setting EOF).  We only care so
		// we can populate stat::st_size.  Just return zero.
		
		return 0;
	}
	
}

