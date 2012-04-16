/*
	attach.cc
	---------
*/

#include "vfs/primitives/attach.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/file_method_set.hh"
#include "vfs/node.hh"
#include "vfs/node_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	void attach( const node* it, const node* target )
	{
		const node_method_set* methods = it->methods();
		
		const file_method_set* file_methods;
		
		if ( methods  &&  (file_methods = methods->file_methods) )
		{
			if ( file_methods->attach )
			{
				file_methods->attach( it, target );
				
				return;
			}
		}
		
		p7::throw_errno( EINVAL );
	}
	
}

