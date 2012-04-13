/*
	gui/new/focuser.hh
	------------------
*/

#ifndef GENIE_FS_GUI_NEW_FOCUSER_HH
#define GENIE_FS_GUI_NEW_FOCUSER_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	FSTreePtr New_focuser( const FSTree*        parent,
	                       const plus::string&  name,
	                       const void*          args );
	
}

#endif

