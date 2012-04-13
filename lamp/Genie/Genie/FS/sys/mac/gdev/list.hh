/*
	Genie/FS/sys/mac/gdev/list.hh
	-----------------------------
*/

#ifndef GENIE_FS_SYS_MAC_GDEV_LIST_HH
#define GENIE_FS_SYS_MAC_GDEV_LIST_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_sys_mac_gdev_list( const FSTree*        parent,
	                                        const plus::string&  name,
	                                        const void*          args );
	
}

#endif

