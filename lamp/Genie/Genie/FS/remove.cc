/*
	remove.cc
	---------
*/

#include "Genie/FS/remove.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/node_method_set.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	void remove( const FSTree* node )
	{
		const node_method_set* methods = node->methods();
		
		if ( methods  &&  methods->remove )
		{
			methods->remove( node );
		}
		else
		{
			p7::throw_errno( EPERM );
		}
	}
	
}

