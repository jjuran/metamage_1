/*	=====================
 *	FSTree_sys_mac_crm.cc
 *	=====================
 */

#if defined( __MACOS__ )  &&  !TARGET_API_MAC_CARBON

#include "Genie/FS/FSTree_sys_mac_crm.hh"

// Genie
#include "Genie/FS/FSTree_crm_serial.hh"


namespace Genie
{
	
	const FSTree_Premapped::Mapping sys_mac_crm_Mappings[] =
	{
		{ "serial", &New_FSTree_sys_mac_crm_serial },
		
		{ NULL, NULL }
	};
	
}

#endif

