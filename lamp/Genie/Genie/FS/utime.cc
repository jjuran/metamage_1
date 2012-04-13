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
	
	
	void utime( const FSTree* it, const struct timespec* times )
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

