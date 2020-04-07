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
	
	static
	void sys_mac_macname_get( plus::var_string& result, const vfs::node* that, bool binary )
	{
		result = GetWorkstationName( false );
	}
	
	static
	void sys_mac_name_get( plus::var_string& result, const vfs::node* that, bool binary )
	{
		result = GetWorkstationName( true );
	}
	
	const vfs::property_params sys_mac_macname_params =
	{
		vfs::no_fixed_size,
		&sys_mac_macname_get,
	};
	
	const vfs::property_params sys_mac_name_params =
	{
		vfs::no_fixed_size,
		&sys_mac_name_get,
	};
	
}
