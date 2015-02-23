/*
	Genie/FS/sys/mac/user/name.hh
	-----------------------------
*/

#ifndef GENIE_FS_SYS_MAC_USER_NAME_HH
#define GENIE_FS_SYS_MAC_USER_NAME_HH

// plus
#include "plus/var_string_fwd.hh"

// vfs
#include "vfs/property.hh"


namespace Genie
{
	
	struct sys_mac_user_macname : vfs::readonly_property
	{
		static void get( plus::var_string& result, const vfs::node* that, bool binary );
	};
	
	struct sys_mac_user_name : vfs::readonly_property
	{
		static void get( plus::var_string& result, const vfs::node* that, bool binary );
	};
	
}

#endif
