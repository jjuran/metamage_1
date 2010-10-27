/*
	premapped.cc
	------------
*/

#include "Genie/FS/premapped.hh"

// Genie
#include "Genie/FS/FSTree_Directory.hh"


namespace Genie
{
	
	FSTreePtr premapped_factory( const FSTreePtr&     parent,
	                             const plus::string&  name,
	                             const void*          args )
	{
		const FSTree_Premapped::Mapping* mappings = (const FSTree_Premapped::Mapping*) args;
		
		return Premapped_Factory( parent, name, mappings );
	}
	
}

