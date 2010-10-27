/*
	Genie/FS/sys/mac/machine.cc
	---------------------------
*/

#include "Genie/FS/sys/mac/machine.hh"

// Genie
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/sys/mac/machine/id.hh"
#include "Genie/FS/sys/mac/machine/name.hh"


namespace Genie
{
	
	#define PROPERTY( prop )  &new_property, &property_params_factory< prop >::value
	
	const FSTree_Premapped::Mapping sys_mac_machine_Mappings[] =
	{
		{ "id",   PROPERTY( sys_mac_machine_id   ) },
		{ "name", PROPERTY( sys_mac_machine_name ) },
		
		{ NULL, NULL }
	};
	
}

