/*	========================
 *	FSTree_sys_mac_errata.hh
 *	========================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSMACERRATA_HH
#define GENIE_FILESYSTEM_FSTREESYSMACERRATA_HH

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"


namespace Genie
{
	
	struct RunningInClassic
	{
		static bool Test();
	};
	
	
	extern const FSTree_Premapped::Mapping sys_mac_errata_Mappings[];
	
}

#endif

