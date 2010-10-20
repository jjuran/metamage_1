/*
	Genie/FS/sys/mac/user/name.hh
	-----------------------------
*/

#ifndef GENIE_FS_SYS_MAC_USER_NAME_HH
#define GENIE_FS_SYS_MAC_USER_NAME_HH


namespace plus
{
	
	class var_string;
	
}

namespace Genie
{
	
	class FSTree;
	
	struct sys_mac_user_name
	{
		static void get( plus::var_string& result, const FSTree* that, bool binary );
	};
	
}

#endif

