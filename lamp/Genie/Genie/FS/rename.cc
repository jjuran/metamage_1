/*
	rename.cc
	---------
*/

#include "Genie/FS/rename.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/node_method_set.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	void rename( const FSTree* it, const FSTree* target )
	{
		const node_method_set* methods = it->methods();
		
		if ( methods  &&  methods->rename )
		{
			methods->rename( it, target );
		}
		else
		{
			p7::throw_errno( EPERM );
		}
	}
	
}

