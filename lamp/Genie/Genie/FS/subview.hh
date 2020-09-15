/*
	subview.hh
	----------
*/

#ifndef GENIE_FS_SUBVIEW_HH
#define GENIE_FS_SUBVIEW_HH

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace Genie
{
	
	vfs::node_ptr subview_factory( const vfs::node*     parent,
	                               const plus::string&  name,
	                               const void*          args );
	
}

#endif
