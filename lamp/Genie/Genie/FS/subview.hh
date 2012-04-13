/*
	subview.hh
	----------
*/

#ifndef GENIE_FS_SUBVIEW_HH
#define GENIE_FS_SUBVIEW_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	FSTreePtr subview_factory( const FSTree*        parent,
	                           const plus::string&  name,
	                           const void*          args );
	
}

#endif

