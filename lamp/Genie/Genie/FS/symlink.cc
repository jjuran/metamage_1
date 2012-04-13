/*
	symlink.cc
	----------
*/

#include "Genie/FS/symlink.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/link_method_set.hh"
#include "Genie/FS/node_method_set.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	void symlink( const FSTree* it, const plus::string& target )
	{
		const node_method_set* methods = it->methods();
		
		const link_method_set* link_methods;
		
		if ( methods  &&  (link_methods = methods->link_methods) )
		{
			if ( link_methods->symlink )
			{
				link_methods->symlink( it, target );
				
				return;
			}
		}
		
		p7::throw_errno( EINVAL );
	}
	
}

