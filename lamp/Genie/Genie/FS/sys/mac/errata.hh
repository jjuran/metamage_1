/*
	Genie/FS/sys/mac/errata.hh
	--------------------------
*/

#ifndef GENIE_FS_SYS_MAC_ERRATA_HH
#define GENIE_FS_SYS_MAC_ERRATA_HH

// Genie
#include "Genie/FS/FSTree_Directory.hh"


namespace Genie
{
	
	struct RunningInClassic
	{
		static bool Test();
	};
	
	
	extern const FSTree_Premapped::Mapping sys_mac_errata_Mappings[];
	
}

#endif

