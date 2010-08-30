/*
	Genie/FS/sys/mac/name.cc
	------------------------
*/

#include "Genie/FS/sys/mac/name.hh"

// Genie
#include "Genie/Utilities/GetWorkstationName.hh"


namespace Genie
{
	
	plus::string sys_mac_name::Read( const FSTree* that, bool binary )
	{
		return GetWorkstationName();
	}
	
}

