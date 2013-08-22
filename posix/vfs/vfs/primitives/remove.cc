/*
	remove.cc
	---------
*/

#include "vfs/primitives/remove.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/methods/node_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	void remove( const node& that )
	{
		const node_method_set* methods = that.methods();
		
		if ( methods  &&  methods->remove )
		{
			methods->remove( &that );
		}
		else
		{
			p7::throw_errno( EPERM );
		}
	}
	
}

