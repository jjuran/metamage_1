/*
	Genie/FS/sys/mac/keys.hh
	------------------------
*/

#ifndef GENIE_FS_SYS_MAC_KEYS_HH
#define GENIE_FS_SYS_MAC_KEYS_HH

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_fwd.hh"


namespace Genie
{
	
	struct sys_mac_keys
	{
		static plus::string Read( const vfs::node* parent, const plus::string& name );
	};
	
}

#endif
