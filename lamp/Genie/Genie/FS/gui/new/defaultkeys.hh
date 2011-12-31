/*
	gui/new/defaultkeys.hh
	----------------------
*/

#ifndef GENIE_FS_GUI_NEW_DEFAULTKEYS_HH
#define GENIE_FS_GUI_NEW_DEFAULTKEYS_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	FSTreePtr New_defaultkeys( const FSTreePtr&     parent,
	                           const plus::string&  name,
	                           const void*          args );
	
}

#endif

