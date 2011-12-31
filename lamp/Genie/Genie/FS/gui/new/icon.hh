/*
	gui/new/icon.hh
	---------------
*/

#ifndef GENIE_FS_GUI_NEW_ICON_HH
#define GENIE_FS_GUI_NEW_ICON_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_new_icon( const FSTreePtr&     parent,
	                               const plus::string&  name,
	                               const void*          args );
	
}

#endif

