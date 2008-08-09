/*	=================
 *	FSTree_sys_app.cc
 *	=================
 */

#include "Genie/FileSystem/FSTree_sys_app.hh"

// Genie
#include "Genie/FileSystem/FSTree_QueryFile.hh"
#include "Genie/FileSystem/FSTree_sys_mac_window.hh"


namespace Genie
{
	
	const Singleton_Mapping sys_app_Mappings[] =
	{
		{ "window", &Singleton_Factory< FSTree_sys_mac_window > },
		
		{ NULL, NULL }
		
	};
	
}

