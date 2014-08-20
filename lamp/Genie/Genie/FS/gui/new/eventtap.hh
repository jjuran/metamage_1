/*
	gui/new/eventtap.hh
	-------------------
*/

#ifndef GENIE_FS_GUI_NEW_EVENTTAP_HH
#define GENIE_FS_GUI_NEW_EVENTTAP_HH

// plus
#include "plus/string_fwd.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace Genie
{
	
	vfs::node_ptr new_eventtap( const vfs::node*     parent,
	                            const plus::string&  name,
	                            const void*          args );
	
}

#endif
