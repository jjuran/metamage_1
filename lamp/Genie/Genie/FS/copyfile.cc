/*
	copyfile.cc
	-----------
*/

#include "Genie/FS/copyfile.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/file_method_set.hh"
#include "Genie/FS/node_method_set.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	void copyfile( const FSTree* node, const FSTree* target )
	{
		const node_method_set* methods = node->methods();
		
		const file_method_set* file_methods;
		
		if ( methods  &&  (file_methods = methods->file_methods) )
		{
			if ( file_methods->copyfile )
			{
				file_methods->copyfile( node, target );
				
				return;
			}
		}
		
		p7::throw_errno( EINVAL );
	}
	
}

