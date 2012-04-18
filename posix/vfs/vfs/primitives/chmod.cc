/*
	chmod.cc
	--------
*/

#include "vfs/primitives/chmod.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "vfs/node.hh"
#include "vfs/methods/node_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	void chmod( const node* it, mode_t mode )
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

