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
	
	#define PROPERTY( prop )  &new_property, &property_params_factory< prop >::value
	
	const FSTree_Premapped::Mapping sys_mac_user_Mappings[] =
	{
		{ "home", &New_FSTree_sys_mac_user_home },
		
		{ "name", PROPERTY( sys_mac_user_name ) },
		
		{ NULL, NULL }
	};
	
}

