/*
	mkdir.cc
	--------
*/

#include "Genie/FS/mkdir.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/dir_method_set.hh"
#include "Genie/FS/node_method_set.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	void mkdir( const FSTree* it, mode_t mode )
	{
		const node_method_set* methods = it->methods();
		
		const dir_method_set* dir_methods;
		
		if ( methods  &&  (dir_methods = methods->dir_methods) )
		{
			if ( dir_methods->mkdir )
			{
				dir_methods->mkdir( it, mode );
				
				return;
			}
		}

		p7::throw_errno( EPERM );
	}
	
}

