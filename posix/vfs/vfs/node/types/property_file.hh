/*
	property_file.hh
	----------------
*/

#ifndef VFS_NODE_TYPES_PROPERTYFILE_HH
#define VFS_NODE_TYPES_PROPERTYFILE_HH

// plus
#include "plus/string_fwd.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace vfs
{
	
	struct undefined_property {};
	
	
	// Can be used in premapped directory maps
	
	node_ptr new_property( const node*          parent,
	                       const plus::string&  name,
	                       const void*          params );
	
}

#endif

