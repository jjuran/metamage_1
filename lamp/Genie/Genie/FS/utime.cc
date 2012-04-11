/*
	utime.cc
	--------
*/

#include "Genie/FS/utime.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/node_method_set.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	void utime( const FSTree* node, const struct timespec* times )
	{
		const node_method_set* methods = node->methods();
		
		if ( methods  &&  methods->utime )
		{
			methods->utime( node, times );
		}
		else
		{
			p7::throw_errno( EPERM );
		}
	}
	
}

