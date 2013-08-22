/*
	attach.cc
	---------
*/

#include "vfs/primitives/attach.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/methods/file_method_set.hh"
#include "vfs/methods/node_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	void attach( const node& that, const node& target )
	{
		const node_method_set* methods = that.methods();
		
		const file_method_set* file_methods;
		
		if ( methods  &&  (file_methods = methods->file_methods) )
		{
			if ( file_methods->attach )
			{
				file_methods->attach( &that, &target );
				
				return;
			}
		}
		
		p7::throw_errno( EINVAL );
	}
	
}

