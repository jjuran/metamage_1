/*
	Genie/FS/sys/mac/user.cc
	------------------------
*/

#include "Genie/FS/sys/mac/user.hh"

// Genie
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/sys/mac/user/home.hh"
#include "Genie/FS/sys/mac/user/name.hh"


namespace Genie
{
	
	static FSTreePtr Name_Factory( const FSTreePtr&     parent,
	                               const plus::string&  name,
	                               const void*          args )
	{
		return New_FSTree_Property( parent,
		                            name,
		                            &sys_mac_user_name::Read );
	}
	
	const FSTree_Premapped::Mapping sys_mac_user_Mappings[] =
	{
		{ "home", &New_FSTree_sys_mac_user_home },
		
		{ "name", &Name_Factory },
		
		{ NULL, NULL }
	};
	
}

