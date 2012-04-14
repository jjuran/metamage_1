/*
	lookup.cc
	---------
*/

#include "vfs/primitives/lookup.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/dir_method_set.hh"
#include "vfs/node.hh"
#include "vfs/node_method_set.hh"
#include "vfs/primitives/parent.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	node_ptr lookup( const node*          it,
	                 const plus::string&  name,
	                 const node*          surrogate )
	{
		if ( name == "." )
		{
			return it;
		}
		else if ( name == ".." )
		{
			return parent( it );
		}
		
		if ( surrogate == NULL )
		{
			surrogate = it;
		}
		
		const node_method_set* methods = it->methods();
		
		const dir_method_set* dir_methods;
		
		if ( methods  &&  (dir_methods = methods->dir_methods) )
		{
			if ( dir_methods->lookup )
			{
				return dir_methods->lookup( it, name, surrogate );
			}
		}
		
		throw p7::errno_t( ENOTDIR );
	}
	
}

