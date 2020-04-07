/*
	Genie/FS/sys/mac/machine.cc
	---------------------------
*/

#include "Genie/FS/sys/mac/machine.hh"

// vfs
#include "vfs/property.hh"
#include "vfs/node/types/property_file.hh"

// Genie
#include "Genie/FS/sys/mac/machine/id.hh"
#include "Genie/FS/sys/mac/machine/name.hh"


namespace Genie
{
	
	#define PROPERTY( prop )  &vfs::new_property, &prop##_params
	
	const vfs::fixed_mapping sys_mac_machine_Mappings[] =
	{
		{ "id",   PROPERTY( sys_mac_machine_id   ) },
		{ "name", PROPERTY( sys_mac_machine_name ) },
		
		{ NULL, NULL }
	};
	
}
