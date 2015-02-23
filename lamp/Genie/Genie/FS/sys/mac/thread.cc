/*
	sys/mac/thread.cc
	-----------------
*/

#include "Genie/FS/sys/mac/thread.hh"

// vfs
#include "vfs/property.hh"
#include "vfs/node/types/property_file.hh"

// Genie
#include "Genie/FS/sys/mac/thread/default-stack-size.hh"


namespace Genie
{
	
	#define PROPERTY( prop )  &vfs::new_property, &vfs::property_params_factory< prop >::value
	
	const vfs::fixed_mapping sys_mac_thread_Mappings[] =
	{
		{ "default-stack-size", PROPERTY( sys_mac_thread_defaultstacksize ) },
		
		{ NULL, NULL }
		
	};
	
}
