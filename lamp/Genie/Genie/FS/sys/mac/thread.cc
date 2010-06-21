/*
	sys/mac/thread.cc
	-----------------
*/

#include "Genie/FS/sys/mac/thread.hh"

// Genie
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/sys/mac/thread/default-stack-size.hh"


namespace Genie
{
	
	#define PROPERTY( prop )  &new_property, &property_params_factory< prop >::value
	
	const FSTree_Premapped::Mapping sys_mac_thread_Mappings[] =
	{
		{ "default-stack-size", PROPERTY( sys_mac_thread_defaultstacksize ) },
		
		{ NULL, NULL }
		
	};
	
}

