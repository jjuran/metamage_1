/*
	Genie/FS/sys/mac/user.cc
	------------------------
*/

#include "Genie/FS/sys/mac/user.hh"

// vfs
#include "vfs/property.hh"
#include "vfs/node/types/property_file.hh"

// Genie
#include "Genie/FS/sys/mac/user/home.hh"
#include "Genie/FS/sys/mac/user/name.hh"
#include "Genie/FS/sys/mac/user/prefs.hh"


namespace Genie
{
	
	#define PROPERTY( prop )  &vfs::new_property, &prop##_params
	
	const vfs::fixed_mapping sys_mac_user_Mappings[] =
	{
		{ "home",  &New_FSTree_sys_mac_user_home  },
		{ "prefs", &New_FSTree_sys_mac_user_prefs },
		
		{ ".mac-name", PROPERTY( sys_mac_user_macname ) },
		{      "name", PROPERTY( sys_mac_user_name    ) },
		
		{ NULL, NULL }
	};
	
}
