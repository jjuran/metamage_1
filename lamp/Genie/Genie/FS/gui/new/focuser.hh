/*
	gui/new/focuser.hh
	------------------
*/

#ifndef GENIE_FS_GUI_NEW_FOCUSER_HH
#define GENIE_FS_GUI_NEW_FOCUSER_HH

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	FSTreePtr New_focuser( const FSTreePtr&     parent,
	                       const plus::string&  name,
	                       const void*          args );
	
}

#endif

