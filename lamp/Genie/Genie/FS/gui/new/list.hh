/*
	gui/new/list.hh
	---------------
*/

#ifndef GENIE_FS_GUI_NEW_LIST_HH
#define GENIE_FS_GUI_NEW_LIST_HH

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_new_list( const FSTreePtr&     parent,
	                               const plus::string&  name,
	                               const void*          args );
	
}

#endif

