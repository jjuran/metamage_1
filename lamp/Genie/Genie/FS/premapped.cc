/*
	premapped.cc
	------------
*/

#include "Genie/FS/premapped.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/FSTree_Directory.hh"


namespace Genie
{
	
	FSTreePtr premapped_factory( const FSTree*        parent,
	                             const plus::string&  name,
	                             const void*          args )
	{
		const vfs::fixed_mapping* mappings = (const vfs::fixed_mapping*) args;
		
		return fixed_dir( parent, name, mappings );
	}
	
}

