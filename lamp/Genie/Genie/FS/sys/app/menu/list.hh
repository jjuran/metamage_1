/*
	Genie/FS/sys/app/menu/list.hh
	-----------------------------
*/

#ifndef GENIE_FS_SYS_APP_MENU_LIST_HH
#define GENIE_FS_SYS_APP_MENU_LIST_HH

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace Genie
{
	
	vfs::node_ptr New_FSTree_sys_app_menu_list( const vfs::node*     parent,
	                                            const plus::string&  name,
	                                            const void*          args );
	
}

#endif
