/*
	opendir.cc
	----------
*/

#include "Genie/FS/opendir.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/dir_method_set.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/IO/VirtualDirectory.hh"


namespace Genie
{
	
	IOPtr opendir( const FSTree* node )
	{
		const node_method_set* methods = node->methods();
		
		const dir_method_set* dir_methods;
		
		if ( methods  &&  (dir_methods = methods->dir_methods) )
		{
			if ( dir_methods->opendir )
			{
				return dir_methods->opendir( node );
			}
		}
		
		return new VirtualDirHandle( node );
	}
	
}

