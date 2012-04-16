/*
	listdir.cc
	----------
*/

#include "vfs/primitives/listdir.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/dir_method_set.hh"
#include "vfs/node_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	void listdir( const node* it, vfs::dir_contents& contents )
	{
		const node_method_set* methods = it->methods();
		
		const dir_method_set* dir_methods;
		
		if ( methods  &&  (dir_methods = methods->dir_methods) )
		{
			if ( dir_methods->listdir )
			{
				dir_methods->listdir( it, contents );
				
				return;
			}
		}

		p7::throw_errno( ENOTDIR );
	}
	
}

