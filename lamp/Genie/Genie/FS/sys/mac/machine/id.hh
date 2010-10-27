/*
	Genie/FS/sys/mac/machine/id.hh
	------------------------------
*/

#ifndef GENIE_FS_SYS_MAC_MACHINE_ID_HH
#define GENIE_FS_SYS_MAC_MACHINE_ID_HH

// Genie
#include "Genie/FS/property.hh"


namespace Genie
{
	
	struct sys_mac_machine_id : readonly_property
	{
		static const std::size_t fixed_size = 4;
		
		static void get( plus::var_string& result, const FSTree* that, bool binary );
	};
	
}

#endif

