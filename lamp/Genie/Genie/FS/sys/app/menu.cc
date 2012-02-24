/*
	Genie/FS/sys/app/menu.cc
	------------------------
*/

#include "Genie/FS/sys/app/menu.hh"

// Genie
#include "Genie/FS/sys/app/menu/list.hh"


namespace Genie
{
	
	const premapped::mapping sys_app_menu_Mappings[] =
	{
		{ "list", &New_FSTree_sys_app_menu_list },
		
		{ NULL, NULL }
		
	};
	
}

