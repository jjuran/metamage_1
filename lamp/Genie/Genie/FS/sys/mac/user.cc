/*
	Genie/FS/sys/mac/user.cc
	------------------------
*/

#include "Genie/FS/sys/mac/user.hh"

// Genie
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/utf8_text_property.hh"
#include "Genie/FS/sys/mac/user/home.hh"
#include "Genie/FS/sys/mac/user/name.hh"
#include "Genie/FS/sys/mac/user/prefs.hh"


namespace Genie
{
	
	#define PROPERTY( prop )  &new_property, &property_params_factory< prop >::value
	
	const FSTree_Premapped::Mapping sys_mac_user_Mappings[] =
	{
		{ "home",  &New_FSTree_sys_mac_user_home  },
		{ "prefs", &New_FSTree_sys_mac_user_prefs },
		
		{ ".mac-name", PROPERTY( sys_mac_user_name ) },
		
		{ "name", PROPERTY( utf8_text_property< sys_mac_user_name > ) },
		
		{ NULL, NULL }
	};
	
}

