/*
	Genie/FS/sys/mac/user.cc
	------------------------
*/

#include "Genie/FS/sys/mac/user.hh"

// Genie
#include "Genie/FS/sys/mac/user/home.hh"
#include "Genie/FS/sys/mac/user/prefs.hh"


namespace Genie
{
	
	const vfs::fixed_mapping sys_mac_user_Mappings[] =
	{
		{ "home",  &New_FSTree_sys_mac_user_home  },
		{ "prefs", &New_FSTree_sys_mac_user_prefs },
		
		{ NULL, NULL }
	};
	
}
