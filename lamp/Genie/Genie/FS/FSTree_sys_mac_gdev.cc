/*	======================
 *	FSTree_sys_mac_gdev.cc
 *	======================
 */

#include "Genie/FS/FSTree_sys_mac_gdev.hh"

// Genie
#include "Genie/FS/sys_mac_gdev_list.hh"


namespace Genie
{
	
	const FSTree_Premapped::Mapping sys_mac_gdev_Mappings[] =
	{
		{ "list", &New_FSTree_sys_mac_gdev_list },
		
		{ NULL, NULL }
		
	};
	
}

