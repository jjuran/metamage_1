/*
	Genie/FS/sys/mac/name.cc
	------------------------
*/

#include "Genie/FS/sys/mac/name.hh"

// plus
#include "plus/var_string.hh"

// Genie
#include "Genie/Utilities/GetWorkstationName.hh"


namespace Genie
{
	
	void sys_mac_name::Read( plus::var_string& result, const FSTree* that, bool binary )
	{
		result = GetWorkstationName();
	}
	
}

