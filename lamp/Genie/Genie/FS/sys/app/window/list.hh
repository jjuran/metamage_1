/*
	Genie/FS/sys/app/window/list.hh
	-------------------------------
*/

#ifndef GENIE_FS_SYS_APP_WINDOW_LIST_HH
#define GENIE_FS_SYS_APP_WINDOW_LIST_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_sys_app_window_list( const FSTreePtr&     parent,
	                                          const plus::string&  name,
	                                          const void*          args );
	
}

#endif

