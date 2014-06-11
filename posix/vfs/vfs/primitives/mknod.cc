/*
	mknod.cc
	--------
*/

#include "vfs/primitives/mknod.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "vfs/node.hh"
#include "vfs/methods/file_method_set.hh"
#include "vfs/methods/node_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	void mknod( const node& that, mode_t mode, dev_t dev )
	{
		const node_method_set* methods = that.methods();
		
		const file_method_set* file_methods;
		
		if ( methods  &&  (file_methods = methods->file_methods) )
		{
			if ( file_methods->mknod )
			{
				file_methods->mknod( &that, mode, dev );
				
				return;
			}
		}
		
		p7::throw_errno( EPERM );
	}
	
}
