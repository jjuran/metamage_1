/*
	parent.cc
	---------
*/

#include "vfs/primitives/parent.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/misc_method_set.hh"
#include "vfs/node_method_set.hh"


namespace vfs
{
	
	node_ptr parent( const node* it )
	{
		if ( it->owner() != NULL )
		{
			return it->owner();
		}
		
		const node_method_set* methods = it->methods();
		
		const misc_method_set* misc_methods;
		
		if ( methods  &&  (misc_methods = methods->misc_methods) )
		{
			if ( misc_methods->parent )
			{
				return misc_methods->parent( it );
			}
		}
		
		return it;
	}
	
}

