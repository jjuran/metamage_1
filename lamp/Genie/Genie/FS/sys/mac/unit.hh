/*
	Genie/FS/sys/mac/unit.hh
	------------------------
*/

#ifndef GENIE_FS_SYS_MAC_UNIT_HH
#define GENIE_FS_SYS_MAC_UNIT_HH

// plus
#include "plus/string_fwd.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace Genie
{
	
	vfs::node_ptr New_FSTree_sys_mac_unit( const vfs::node*     parent,
	                                       const plus::string&  name,
	                                       const void*          args );
	
}

#endif
