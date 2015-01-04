/*
	utime.cc
	--------
*/

#include "vfs/primitives/utime.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/methods/item_method_set.hh"
#include "vfs/methods/node_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	void utime( const node& that, const struct timespec* times )
	{
		const node_method_set* methods = that.methods();
		
		const item_method_set* item_methods;
		
		if ( methods  &&  (item_methods = methods->item_methods) )
		{
			if ( item_methods->utime )
			{
				item_methods->utime( &that, times );
				
				return;
			}
		}
		
		p7::throw_errno( EPERM );
	}
	
}
