/*
	parent.cc
	---------
*/

#include "Genie/FS/parent.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/misc_method_set.hh"
#include "Genie/FS/node_method_set.hh"


namespace Genie
{
	
	FSTreePtr parent( const FSTree* it )
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

