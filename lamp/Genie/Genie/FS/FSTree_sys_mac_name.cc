/*	======================
 *	FSTree_sys_mac_name.cc
 *	======================
 */

#include "Genie/FS/FSTree_sys_mac_name.hh"

// Genie
#include "Genie/Utilities/GetWorkstationName.hh"


namespace Genie
{
	
	plus::string sys_mac_name::Read( const FSTree* that, bool binary )
	{
		return GetWorkstationName();
	}
	
}

