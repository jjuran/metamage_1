/*
	chmod.cc
	--------
*/

#include "vfs/primitives/chmod.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "vfs/node.hh"
#include "vfs/methods/item_method_set.hh"
#include "vfs/methods/node_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	void chmod( const node& that, mode_t mode )
	{
		const node_method_set* methods = that.methods();
		
		const item_method_set* item_methods;
		
		if ( methods  &&  (item_methods = methods->item_methods) )
		{
			if ( item_methods->chmod )
			{
				item_methods->chmod( &that, mode );
				
				return;
			}
		}
		
		p7::throw_errno( EPERM );
	}
	
}
