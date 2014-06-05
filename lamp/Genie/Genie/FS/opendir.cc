/*
	opendir.cc
	----------
*/

// vfs
#include "vfs/node.hh"
#include "vfs/methods/dir_method_set.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/primitives/opendir.hh"
#include "vfs/filehandle/types/directory.hh"


namespace vfs
{
	
	filehandle_ptr opendir( const node& that )
	{
		const node_method_set* methods = that.methods();
		
		const dir_method_set* dir_methods;
		
		if ( methods  &&  (dir_methods = methods->dir_methods) )
		{
			if ( dir_methods->opendir )
			{
				return dir_methods->opendir( &that );
			}
		}
		
		return new dir_handle( &that );
	}
	
}

