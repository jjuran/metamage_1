/*
	Genie/FS/sys/mac/name.hh
	------------------------
*/

#ifndef GENIE_FS_SYS_MAC_NAME_HH
#define GENIE_FS_SYS_MAC_NAME_HH

// Genie
#include "Genie/FS/property.hh"


namespace Genie
{
	
	struct sys_mac_name : readonly_property
	{
		static void get( plus::var_string& result, const FSTree* that, bool binary );
	};
	
}

#endif

