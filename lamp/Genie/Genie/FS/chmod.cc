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
	
	
	void chmod( const FSTree* it, mode_t mode )
	{
		const node_method_set* methods = it->methods();
		
		if ( methods  &&  methods->chmod )
		{
			methods->chmod( it, mode );
		}
		else
		{
			p7::throw_errno( EPERM );
		}
	}
	
}

