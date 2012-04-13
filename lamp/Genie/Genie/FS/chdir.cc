/*
	chdir.cc
	--------
*/

#include "Genie/FS/chdir.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/dir_method_set.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/FS/opendir.hh"
#include "Genie/IO/Base.hh"


namespace Genie
{
	
	IOPtr chdir( const FSTree* it )
	{
		const node_method_set* methods = it->methods();
		
		const dir_method_set* dir_methods;
		
		if ( methods  &&  (dir_methods = methods->dir_methods) )
		{
			if ( dir_methods->chdir )
			{
				return dir_methods->chdir( it );
			}
		}
		
		return opendir( it );
	}
	
}

