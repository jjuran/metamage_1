/*
	chmod.cc
	--------
*/

#include "Genie/FS/chmod.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/node_method_set.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	void chmod( const FSTree* node, mode_t mode )
	{
		const node_method_set* methods = node->methods();
		
		if ( methods  &&  methods->chmod )
		{
			methods->chmod( node, mode );
		}
		else
		{
			p7::throw_errno( EPERM );
		}
	}
	
}

