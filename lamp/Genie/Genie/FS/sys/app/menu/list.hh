/*
	Genie/FS/sys/app/menu/list.hh
	-----------------------------
*/

#ifndef GENIE_FS_SYS_APP_MENU_LIST_HH
#define GENIE_FS_SYS_APP_MENU_LIST_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_sys_app_menu_list( const FSTree*        parent,
	                                        const plus::string&  name,
	                                        const void*          args );
	
}

#endif

