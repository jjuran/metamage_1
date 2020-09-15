/*
	gui/new/port.hh
	---------------
*/

#ifndef GENIE_FS_GUI_NEW_PORT_HH
#define GENIE_FS_GUI_NEW_PORT_HH

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace Genie
{
	
	vfs::node_ptr New_new_port( const vfs::node*     parent,
	                            const plus::string&  name,
	                            const void*          args );
	
}

#endif
