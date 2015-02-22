/*
	gui/new/scroller.hh
	-------------------
*/

#ifndef GENIE_FS_GUI_NEW_SCROLLER_HH
#define GENIE_FS_GUI_NEW_SCROLLER_HH

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace Genie
{
	
	vfs::node_ptr New_FSTree_new_scroller( const vfs::node*     parent,
	                                       const plus::string&  name,
	                                       const void*          args );
	
}

#endif
