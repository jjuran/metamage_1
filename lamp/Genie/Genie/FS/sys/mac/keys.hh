/*
	Genie/FS/sys/mac/keys.hh
	------------------------
*/

#ifndef GENIE_FS_SYS_MAC_KEYS_HH
#define GENIE_FS_SYS_MAC_KEYS_HH

// plus
#include "plus/string.hh"


namespace Genie
{
	
	class FSTree;
	
	struct sys_mac_keys
	{
		static plus::string Read( const FSTree* );
	};
	
}

#endif

