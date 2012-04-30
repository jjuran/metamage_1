/*
	Genie/FS/gui/port.hh
	--------------------
*/

#ifndef GENIE_FS_GUI_PORT_HH
#define GENIE_FS_GUI_PORT_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	FSTreePtr new_port();
	
	void remove_port( const FSTree* port );
	
	
	FSTreePtr new_gui_port( const FSTree*        parent,
	                        const plus::string&  name,
	                        const void*          args );
	
}

#endif

