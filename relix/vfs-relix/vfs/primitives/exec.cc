/*
	exec.cc
	-------
*/

#include "vfs/primitives/exec.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/methods/file_method_set.hh"
#include "vfs/methods/node_method_set.hh"

// vfs-relix
#include "vfs/program.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	program_ptr exec( const node& that )
	{
		const node_method_set* methods = that.methods();
		
		const file_method_set* file_methods;
		
		if ( methods  &&  (file_methods = methods->file_methods) )
		{
			if ( file_methods->loadexec )
			{
				return file_methods->loadexec( &that );
			}
		}
		
		throw p7::errno_t( ENOEXEC );
	}
	
}

