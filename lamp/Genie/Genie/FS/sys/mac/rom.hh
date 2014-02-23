/*
	Genie/FS/sys/mac/rom.hh
	-----------------------
*/

#ifndef GENIE_FS_SYS_MAC_ROM_HH
#define GENIE_FS_SYS_MAC_ROM_HH

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace vfs
{
	
	node_ptr new_sys_mac_rom( const node*          parent,
	                          const plus::string&  name,
	                          const void*          args );
	
}

#endif

