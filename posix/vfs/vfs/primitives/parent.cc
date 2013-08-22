/*
	parent.cc
	---------
*/

#include "vfs/primitives/parent.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/methods/misc_method_set.hh"
#include "vfs/methods/node_method_set.hh"


namespace vfs
{
	
	node_ptr parent( const node& that )
	{
		if ( that.owner() != NULL )
		{
			return that.owner();
		}
		
		const node_method_set* methods = that.methods();
		
		const misc_method_set* misc_methods;
		
		if ( methods  &&  (misc_methods = methods->misc_methods) )
		{
			if ( misc_methods->parent )
			{
				return misc_methods->parent( &that );
			}
		}
		
		return &that;
	}
	
}

