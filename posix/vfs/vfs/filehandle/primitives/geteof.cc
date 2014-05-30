/*
	geteof.cc
	---------
*/

#include "vfs/filehandle/primitives/geteof.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/filehandle/methods/bstore_method_set.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/primitives/get_file.hh"
#include "vfs/primitives/geteof.hh"


namespace vfs
{
	
	off_t geteof( filehandle& that )
	{
		const vfs::bstore_method_set& bstore_methods = that.bstore_methods();
		
		if ( bstore_methods.geteof == NULL )
		{
			return geteof( *get_file( that ) );
		}
		
		return bstore_methods.geteof( &that );
	}
	
}

