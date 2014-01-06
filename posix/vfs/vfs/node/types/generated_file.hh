/*
	generated_file.hh
	-----------------
*/

#ifndef VFS_NODE_TYPES_GENERATEDFILE_HH
#define VFS_NODE_TYPES_GENERATEDFILE_HH

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace vfs
{
	
	typedef plus::string (*generated_file_hook)( const node* parent, const plus::string& name );
	
	// Can be used in premapped directory maps
	
	node_ptr new_generated( const node*          parent,
	                        const plus::string&  name,
	                        const void*          params );
	
}

#endif

