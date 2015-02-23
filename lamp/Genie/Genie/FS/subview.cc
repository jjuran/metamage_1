/*
	subview.cc
	----------
*/

#include "Genie/FS/subview.hh"

// vfs
#include "vfs/node.hh"

// Genie
#include "Genie/FS/Views.hh"


namespace Genie
{
	
	vfs::node_ptr subview_factory( const vfs::node*     parent,
	                               const plus::string&  name,
	                               const void*          args )
	{
		ViewGetter getter = (ViewGetter) args;  // reinterpret_cast
		
		return New_View( parent, name, getter, NULL );
	}
	
}
