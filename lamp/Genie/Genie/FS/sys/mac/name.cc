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
	
	void sys_mac_macname::get( plus::var_string& result, const FSTree* that, bool binary )
	{
		result = GetWorkstationName( false );
	}
	
	void sys_mac_name::get( plus::var_string& result, const FSTree* that, bool binary )
	{
		result = GetWorkstationName( true );
	}
	
}

