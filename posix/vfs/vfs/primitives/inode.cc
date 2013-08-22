/*
	inode.cc
	--------
*/

#include "vfs/primitives/inode.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/methods/misc_method_set.hh"
#include "vfs/methods/node_method_set.hh"


namespace vfs
{
	
	ino_t inode( const node& that )
	{
		const node_method_set* methods = that.methods();
		
		const misc_method_set* misc_methods;
		
		if ( methods  &&  (misc_methods = methods->misc_methods) )
		{
			if ( misc_methods->inode )
			{
				return misc_methods->inode( &that );
			}
		}
		
		return 0;
	}
	
}

