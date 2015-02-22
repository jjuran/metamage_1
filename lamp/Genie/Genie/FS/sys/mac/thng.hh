/*
	Genie/FS/sys/mac/thng.hh
	------------------------
*/

#ifndef GENIE_FS_SYS_MAC_THNG_HH
#define GENIE_FS_SYS_MAC_THNG_HH

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace Genie
{
	
	vfs::node_ptr New_FSTree_sys_mac_thng( const vfs::node*     parent,
	                                       const plus::string&  name,
	                                       const void*          args );
	
}

#endif
