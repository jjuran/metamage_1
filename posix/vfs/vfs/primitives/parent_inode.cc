/*
	parent_inode.cc
	---------------
*/

#include "vfs/primitives/parent_inode.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/methods/misc_method_set.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/primitives/inode.hh"
#include "vfs/primitives/parent.hh"


namespace vfs
{
	
	ino_t parent_inode( const node& that )
	{
		const node_method_set* methods = that.methods();
		
		const misc_method_set* misc_methods;
		
		if ( methods  &&  (misc_methods = methods->misc_methods) )
		{
			if ( misc_methods->parent_inode )
			{
				return misc_methods->parent_inode( &that );
			}
		}
		
		return inode( *parent( that ) );
	}
	
}

