/*
	inode.cc
	--------
*/

#include "Genie/FS/inode.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/misc_method_set.hh"
#include "Genie/FS/node_method_set.hh"


namespace Genie
{
	
	ino_t inode( const FSTree* node )
	{
		const node_method_set* methods = node->methods();
		
		const misc_method_set* misc_methods;
		
		if ( methods  &&  (misc_methods = methods->misc_methods) )
		{
			if ( misc_methods->inode )
			{
				return misc_methods->inode( node );
			}
		}
		
		return 0;
	}
	
}

