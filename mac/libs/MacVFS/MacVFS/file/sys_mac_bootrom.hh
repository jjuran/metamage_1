/*
	sys_mac_bootrom.hh
	------------------
*/

#ifndef MACVFS_FILE_SYSMACBOOTROM_HH
#define MACVFS_FILE_SYSMACBOOTROM_HH

// plus
#include "plus/string_fwd.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace vfs
{
	
	node_ptr new_sys_mac_bootrom( const node*          parent,
	                              const plus::string&  name,
	                              const void*          args );
	
}

#endif
