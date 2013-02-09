/*
	opendir.hh
	----------
*/

// vfs
#include "vfs/filehandle_ptr.hh"
#include "vfs/node_fwd.hh"


namespace vfs
{
	
	filehandle_ptr opendir( const node* that );
	
}

