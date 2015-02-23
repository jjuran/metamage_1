/*
	Genie/FS/sys/mac/user/prefs.hh
	------------------------------
*/

#ifndef GENIE_FS_SYS_MAC_USER_PREFS_HH
#define GENIE_FS_SYS_MAC_USER_PREFS_HH

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace Genie
{
	
	vfs::node_ptr New_FSTree_sys_mac_user_prefs( const vfs::node*     parent,
	                                             const plus::string&  name,
	                                             const void*          args );
	
}

#endif
