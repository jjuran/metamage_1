/*
	listdir.cc
	----------
*/

#include "Genie/FS/listdir.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/dir_method_set.hh"
#include "Genie/FS/node_method_set.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	void listdir( const FSTree* node, vfs::dir_contents& contents )
	{
		const node_method_set* methods = node->methods();
		
		const dir_method_set* dir_methods;
		
		if ( methods  &&  (dir_methods = methods->dir_methods) )
		{
			if ( dir_methods->listdir )
			{
				dir_methods->listdir( node, contents );
				
				return;
			}
		}

		p7::throw_errno( ENOTDIR );
	}
	
}

