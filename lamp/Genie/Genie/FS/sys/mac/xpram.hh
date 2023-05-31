/*
	Genie/FS/sys/mac/xpram.hh
	-------------------------
*/

#ifndef GENIE_FS_SYS_MAC_XPRAM_HH
#define GENIE_FS_SYS_MAC_XPRAM_HH

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_fwd.hh"


namespace Genie
{
	
	struct sys_mac_xpram
	{
		static plus::string Read( const vfs::node* parent, const plus::string& name );
	};
	
}

#endif
