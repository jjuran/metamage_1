/*
	Genie/FS/sys/mac/vol.cc
	-----------------------
*/

#include "Genie/FS/sys/mac/vol.hh"

// Genie
#include "Genie/FS/sys/mac/vol/list.hh"


namespace Genie
{
	
	const vfs::fixed_mapping sys_mac_vol_Mappings[] =
	{
		{ "list", &New_FSTree_sys_mac_vol },
		
		{ NULL, NULL }
		
	};
	
}
