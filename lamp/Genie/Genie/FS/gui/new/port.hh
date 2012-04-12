/*
	gui/new/port.hh
	---------------
*/

#ifndef GENIE_FS_GUI_NEW_PORT_HH
#define GENIE_FS_GUI_NEW_PORT_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	FSTreePtr New_new_port( const FSTree*        parent,
	                        const plus::string&  name,
	                        const void*          args );
	
}

#endif

