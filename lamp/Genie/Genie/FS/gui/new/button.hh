/*
	gui/new/button.hh
	-----------------
*/

#ifndef GENIE_FS_GUI_NEW_BUTTON_HH
#define GENIE_FS_GUI_NEW_BUTTON_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_new_button( const FSTreePtr&     parent,
	                                 const plus::string&  name,
	                                 const void*          args );
	
}

#endif

