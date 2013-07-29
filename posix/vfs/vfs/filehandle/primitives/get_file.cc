/*
	get_file.cc
	----------
*/

#include "vfs/filehandle/primitives/get_file.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"


namespace vfs
{
	
	node_ptr get_file( filehandle& that )
	{
		node_ptr file = that.GetFile();
		
		return file;
	}
	
}
