/*
	Genie/FS/sys/mac/gestalt.hh
	---------------------------
*/

#ifndef GENIE_FS_SYS_MAC_GESTALT_HH
#define GENIE_FS_SYS_MAC_GESTALT_HH

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace Genie
{
	
	vfs::node_ptr New_FSTree_sys_mac_gestalt( const vfs::node*     parent,
	                                          const plus::string&  name,
	                                          const void*          args );
	
}

#endif
