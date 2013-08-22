/*
	lookup.cc
	---------
*/

#include "vfs/primitives/lookup.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/methods/dir_method_set.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/primitives/parent.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	node_ptr lookup( const node&          that,
	                 const plus::string&  name,
	                 const node*          surrogate )
	{
		if ( name == "." )
		{
			return &that;
		}
		else if ( name == ".." )
		{
			return parent( that );
		}
		
		if ( surrogate == NULL )
		{
			surrogate = &that;
		}
		
		const node_method_set* methods = that.methods();
		
		const dir_method_set* dir_methods;
		
		if ( methods  &&  (dir_methods = methods->dir_methods) )
		{
			if ( dir_methods->lookup )
			{
				return dir_methods->lookup( &that, name, surrogate );
			}
		}
		
		throw p7::errno_t( ENOTDIR );
	}
	
}

