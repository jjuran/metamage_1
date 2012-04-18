/*	===================
 *	FSTree_Directory.cc
 *	===================
 */

#include "Genie/FS/FSTree_Directory.hh"

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	FSTreePtr Premapped_Factory( const FSTree*               parent,
	                             const plus::string&         name,
	                             const premapped::mapping    mappings[],
	                             void                      (*dtor)(const FSTree*) )
	{
		return fixed_dir( parent, name, mappings, dtor );
	}
	
}

