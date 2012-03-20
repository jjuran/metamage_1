/*
	subview.cc
	----------
*/

#include "Genie/FS/subview.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/Views.hh"


namespace Genie
{
	
	FSTreePtr subview_factory( const FSTreePtr&     parent,
	                           const plus::string&  name,
	                           const void*          args )
	{
		ViewGetter getter = (ViewGetter) args;  // reinterpret_cast
		
		return New_View( parent, name, getter, NULL );
	}
	
}

