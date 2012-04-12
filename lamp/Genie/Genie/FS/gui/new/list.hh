/*
	gui/new/list.hh
	---------------
*/

#ifndef GENIE_FS_GUI_NEW_LIST_HH
#define GENIE_FS_GUI_NEW_LIST_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_new_list( const FSTree*        parent,
	                               const plus::string&  name,
	                               const void*          args );
	
}

#endif

