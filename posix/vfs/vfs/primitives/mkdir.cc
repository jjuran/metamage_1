/*
	mkdir.cc
	--------
*/

#include "vfs/primitives/mkdir.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/methods/dir_method_set.hh"
#include "vfs/methods/node_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	void mkdir( const node* it, mode_t mode )
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

