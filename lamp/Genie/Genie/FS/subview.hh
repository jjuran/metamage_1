/*
	subview.hh
	----------
*/

#ifndef GENIE_FS_SUBVIEW_HH
#define GENIE_FS_SUBVIEW_HH

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	FSTreePtr subview_factory( const FSTreePtr&     parent,
	                           const plus::string&  name,
	                           const void*          args );
	
}

#endif

