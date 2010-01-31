/*	==============================
 *	FSTree_sys_mac_machine_name.cc
 *	==============================
 */

#include "Genie/FS/FSTree_sys_mac_machine_name.hh"

// Genie
#include "Genie/Utilities/GetMachineName.hh"


namespace Genie
{
	
	std::string sys_mac_machine_name::Read( const FSTree* that, bool binary )
	{
		const unsigned char* name = GetMachineName();
		
		return std::string( (const char*) &name[1], name[0] );
	}
	
}

