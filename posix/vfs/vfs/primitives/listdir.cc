/*
	listdir.cc
	----------
*/

#include "vfs/primitives/listdir.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/methods/dir_method_set.hh"
#include "vfs/methods/node_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	void listdir( const node& that, vfs::dir_contents& contents )
	{
		const node_method_set* methods = that.methods();
		
		const dir_method_set* dir_methods;
		
		if ( methods  &&  (dir_methods = methods->dir_methods) )
		{
			if ( dir_methods->listdir )
			{
				dir_methods->listdir( &that, contents );
				
				return;
			}
		}

		p7::throw_errno( ENOTDIR );
	}
	
}

