/*
	Genie/FS/sys/mac/machine/name.hh
	--------------------------------
*/

#ifndef GENIE_FS_SYS_MAC_MACHINE_NAME_HH
#define GENIE_FS_SYS_MAC_MACHINE_NAME_HH

// vfs
#include "vfs/property.hh"


namespace Genie
{
	
	struct sys_mac_machine_name : vfs::readonly_property
	{
		static void get( plus::var_string& result, const vfs::node* that, bool binary );
	};
	
}

#endif
