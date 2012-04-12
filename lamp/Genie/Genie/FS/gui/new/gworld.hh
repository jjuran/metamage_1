/*
	gui/new/gworld.hh
	-----------------
*/

#ifndef GENIE_FS_GUI_NEW_GWORLD_HH
#define GENIE_FS_GUI_NEW_GWORLD_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_new_gworld( const FSTree*        parent,
	                                 const plus::string&  name,
	                                 const void* );
	
}

#endif

