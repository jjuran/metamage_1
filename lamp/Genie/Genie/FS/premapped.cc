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
	
	FSTreePtr premapped_factory( const FSTreePtr&     parent,
	                             const plus::string&  name,
	                             const void*          args )
	{
		const premapped::mapping* mappings = (const premapped::mapping*) args;
		
		return Premapped_Factory( parent, name, mappings );
	}
	
}

