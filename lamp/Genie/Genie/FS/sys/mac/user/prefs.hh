/*
	Genie/FS/sys/mac/user/prefs.hh
	------------------------------
*/

#ifndef GENIE_FS_SYS_MAC_USER_PREFS_HH
#define GENIE_FS_SYS_MAC_USER_PREFS_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_sys_mac_user_prefs( const FSTree*        parent,
	                                         const plus::string&  name,
	                                         const void*          args );
	
}

#endif

