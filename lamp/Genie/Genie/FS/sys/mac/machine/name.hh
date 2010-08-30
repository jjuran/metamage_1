/*
	Genie/FS/sys/mac/machine/name.hh
	--------------------------------
*/

#ifndef GENIE_FS_SYS_MAC_MACHINE_NAME_HH
#define GENIE_FS_SYS_MAC_MACHINE_NAME_HH

// plus
#include "plus/string.hh"


namespace Genie
{
	
	class FSTree;
	
	struct sys_mac_machine_name
	{
		static plus::string Read( const FSTree* that, bool binary );
	};
	
}

#endif

