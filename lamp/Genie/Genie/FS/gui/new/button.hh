/*
	gui/new/button.hh
	-----------------
*/

#ifndef GENIE_FS_GUI_NEW_BUTTON_HH
#define GENIE_FS_GUI_NEW_BUTTON_HH

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace Genie
{
	
	vfs::node_ptr New_FSTree_new_button( const vfs::node*     parent,
	                                     const plus::string&  name,
	                                     const void*          args );
	
}

#endif
