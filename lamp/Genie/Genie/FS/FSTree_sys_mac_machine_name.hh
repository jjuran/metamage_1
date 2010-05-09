/*	==============================
 *	FSTree_sys_mac_machine_name.hh
 *	==============================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSMACMACHINENAME_HH
#define GENIE_FILESYSTEM_FSTREESYSMACMACHINENAME_HH

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

