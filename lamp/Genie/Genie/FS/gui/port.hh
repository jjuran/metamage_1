/*
	Genie/FS/gui/port.hh
	--------------------
*/

#ifndef GENIE_FS_GUI_PORT_HH
#define GENIE_FS_GUI_PORT_HH

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace Genie
{
	
	vfs::node_ptr new_port();
	
	void remove_port( const vfs::node* port );
	
	
	vfs::node_ptr new_gui_port( const vfs::node*     parent,
	                            const plus::string&  name,
	                            const void*          args );
	
}

#endif
