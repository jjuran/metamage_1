/*
	get_file.cc
	----------
*/

#include "vfs/filehandle/primitives/get_file.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/general_method_set.hh"


namespace vfs
{
	
	node_ptr get_file( filehandle& that )
	{
		if ( const filehandle_method_set* methods = that.methods() )
		{
			if ( const general_method_set* general_methods = methods->general_methods )
			{
				if ( general_methods->getfile )
				{
					return general_methods->getfile( &that );
				}
			}
		}
		
		node_ptr file = that.GetFile();
		
		return file;
	}
	
}
