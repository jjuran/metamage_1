/*	==================
 *	FSTree_Property.hh
 *	==================
 */

#ifndef GENIE_FILESYSTEM_FSTREE_PROPERTY_HH
#define GENIE_FILESYSTEM_FSTREE_PROPERTY_HH

// plus
#include "plus/string_fwd.hh"

// vfs
#include "vfs/node_ptr.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace vfs
{
	
	struct undefined_property {};
	
	
	// Can be used in premapped directory maps
	
	node_ptr new_property( const node*          parent,
	                       const plus::string&  name,
	                       const void*          params );
	
}

namespace Genie
{
	
	using vfs::undefined_property;
	using vfs::new_property;
	
}

#endif

