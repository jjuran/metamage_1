/*
	sys_mac_vol.cc
	--------------
	
	Copyright 2009, Joshua Juran
*/

#include "Genie/FS/sys_mac_vol.hh"

// Genie
#include "Genie/FS/sys_mac_vol_list.hh"


namespace Genie
{
	
	const FSTree_Premapped::Mapping sys_mac_vol_Mappings[] =
	{
		{ "list", &New_FSTree_sys_mac_vol },
		
		{ NULL, NULL }
		
	};
	
}

