/*	============================
 *	FSTree_sys_mac_machine_id.hh
 *	============================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSMACMACHINEID_HH
#define GENIE_FILESYSTEM_FSTREESYSMACMACHINEID_HH

// Standard C++
#include <string>


namespace Genie
{
	
	class FSTree;
	
	struct sys_mac_machine_id
	{
		static std::string Read( const FSTree* );
	};
	
}

#endif

