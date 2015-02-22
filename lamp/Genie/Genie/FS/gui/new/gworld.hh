/*
	gui/new/gworld.hh
	-----------------
*/

#ifndef GENIE_FS_GUI_NEW_GWORLD_HH
#define GENIE_FS_GUI_NEW_GWORLD_HH

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace Genie
{
	
	vfs::node_ptr New_FSTree_new_gworld( const vfs::node*     parent,
	                                     const plus::string&  name,
	                                     const void* );
	
}

#endif
