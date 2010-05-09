/*	============================
 *	FSTree_sys_mac_machine_id.hh
 *	============================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSMACMACHINEID_HH
#define GENIE_FILESYSTEM_FSTREESYSMACMACHINEID_HH

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

