/*
	remove.cc
	---------
*/

#include "vfs/primitives/remove.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/methods/item_method_set.hh"
#include "vfs/methods/node_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	void remove( const node& that )
	{
		const node_method_set* methods = that.methods();
		
		const item_method_set* item_methods;
		
		if ( methods  &&  (item_methods = methods->item_methods) )
		{
			if ( item_methods->remove )
			{
				item_methods->remove( &that );
				
				return;
			}
		}
		
		p7::throw_errno( EPERM );
	}
	
}
