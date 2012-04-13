/*
	Genie/FS/sys/mac/user/home.hh
	-----------------------------
*/

#ifndef GENIE_FS_SYS_MAC_USER_HOME_HH
#define GENIE_FS_SYS_MAC_USER_HOME_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_sys_mac_user_home( const FSTree*        parent,
	                                        const plus::string&  name,
	                                        const void*          args );
	
}

#endif

