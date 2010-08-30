/*
	Genie/FS/sys/mac/machine/id.hh
	------------------------------
*/

#ifndef GENIE_FS_SYS_MAC_MACHINE_ID_HH
#define GENIE_FS_SYS_MAC_MACHINE_ID_HH

// plus
#include "plus/string.hh"


namespace Genie
{
	
	class FSTree;
	
	struct sys_mac_machine_id
	{
		static plus::string Read( const FSTree* that, bool binary );
	};
	
}

#endif

