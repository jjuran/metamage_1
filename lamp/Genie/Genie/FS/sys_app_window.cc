/*	=================
 *	sys_app_window.cc
 *	=================
 */

#include "Genie/FS/sys_app_window.hh"

// Genie
#include "Genie/FS/FSTree_sys_mac_window.hh"


namespace Genie
{
	
	const FSTree_Premapped::Mapping sys_app_window_Mappings[] =
	{
		{ "list", &Basic_Factory< FSTree_sys_mac_window > },
		
		{ NULL, NULL }
		
	};
	
}

