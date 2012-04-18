/*
	utime.cc
	--------
*/

#include "vfs/primitives/utime.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/methods/node_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	void utime( const node* it, const struct timespec* times )
	{
		const node_method_set* methods = it->methods();
		
		if ( methods  &&  methods->utime )
		{
			methods->utime( it, times );
		}
		else
		{
			p7::throw_errno( EPERM );
		}
	}
	
}

