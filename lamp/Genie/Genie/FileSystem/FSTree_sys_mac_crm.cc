/*	=====================
 *	FSTree_sys_mac_crm.cc
 *	=====================
 */

#if !TARGET_API_MAC_CARBON

#include "Genie/FileSystem/FSTree_sys_mac_crm.hh"

// Genie
#include "Genie/FileSystem/FSTree_crm_serial.hh"


namespace Genie
{
	
	const FSTree_Premapped::Mapping sys_mac_crm_Mappings[] =
	{
		{ "serial", &Basic_Factory< FSTree_sys_mac_crm_serial > },
		
		{ NULL, NULL }
	};
	
}

#endif

