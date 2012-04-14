/*
	parent_inode.cc
	---------------
*/

#include "Genie/FS/parent_inode.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/primitives/inode.hh"
#include "vfs/primitives/parent.hh"

// Genie
#include "Genie/FS/misc_method_set.hh"
#include "Genie/FS/node_method_set.hh"


namespace Genie
{
	
	ino_t parent_inode( const FSTree* it )
	{
		const node_method_set* methods = it->methods();
		
		const misc_method_set* misc_methods;
		
		if ( methods  &&  (misc_methods = methods->misc_methods) )
		{
			if ( misc_methods->parent_inode )
			{
				return misc_methods->parent_inode( it );
			}
		}
		
		return inode( parent( it ).get() );
	}
	
}

