/*
	sys_mac_rom.hh
	--------------
*/

#ifndef MACVFS_FILE_SYSMACROM_HH
#define MACVFS_FILE_SYSMACROM_HH

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

