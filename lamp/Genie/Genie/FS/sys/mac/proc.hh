/*
	Genie/FS/sys/mac/proc.hh
	------------------------
*/

#ifndef GENIE_FS_SYS_MAC_PROC_HH
#define GENIE_FS_SYS_MAC_PROC_HH

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace Genie
{
	
	vfs::node_ptr New_FSTree_sys_mac_proc( const vfs::node*     parent,
	                                       const plus::string&  name,
	                                       const void*          args );
	
}

#endif
