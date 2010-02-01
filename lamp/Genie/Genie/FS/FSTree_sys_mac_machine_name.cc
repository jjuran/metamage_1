/*	==============================
 *	FSTree_sys_mac_machine_name.cc
 *	==============================
 */

#include "Genie/FS/FSTree_sys_mac_machine_name.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/Utilities/GetMachineName.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	std::string sys_mac_machine_name::Read( const FSTree* that, bool binary )
	{
		const unsigned char* name = GetMachineName();
		
		if ( name == NULL )
		{
			p7::throw_errno( ENOENT );
		}
		
		return std::string( (const char*) &name[1], name[0] );
	}
	
}

