/*
	Genie/FS/sys/mac/crm.cc
	-----------------------
*/

#if defined( __MACOS__ )  &&  !TARGET_API_MAC_CARBON

#include "Genie/FS/sys/mac/crm.hh"

// Genie
#include "Genie/FS/sys/mac/crm/serial.hh"


namespace Genie
{
	
	const FSTree_Premapped::Mapping sys_mac_crm_Mappings[] =
	{
		{ "serial", &New_FSTree_sys_mac_crm_serial },
		
		{ NULL, NULL }
	};
	
}

#endif

