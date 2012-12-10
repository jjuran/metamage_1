/*
	Genie/FS/sys/mac/user/name.hh
	-----------------------------
*/

#ifndef GENIE_FS_SYS_MAC_USER_NAME_HH
#define GENIE_FS_SYS_MAC_USER_NAME_HH

// plus
#include "plus/var_string_fwd.hh"

// Genie
#include "Genie/FS/FSTree_fwd.hh"
#include "Genie/FS/property.hh"


namespace Genie
{
	
	struct sys_mac_user_macname : readonly_property
	{
		static void get( plus::var_string& result, const FSTree* that, bool binary );
	};
	
	struct sys_mac_user_name : readonly_property
	{
		static void get( plus::var_string& result, const FSTree* that, bool binary );
	};
	
}

#endif

