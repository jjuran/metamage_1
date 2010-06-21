/*
	sys/mac/time.cc
	---------------
*/

#include "Genie/FS/sys/mac/time.hh"

// Genie
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/sys/mac/time/dls+gmt-delta.hh"


namespace Genie
{
	
	#define PROPERTY( prop )  &new_property, &property_params_factory< prop >::value
	
	const FSTree_Premapped::Mapping sys_mac_time_Mappings[] =
	{
		{ ".dls+gmt-delta", PROPERTY( sys_mac_time_dlsgmtdelta ) },
		{ "dls",            PROPERTY( sys_mac_time_dls         ) },
		{ "gmt-delta",      PROPERTY( sys_mac_time_gmtdelta    ) },
		
		{ NULL, NULL }
		
	};
	
}

