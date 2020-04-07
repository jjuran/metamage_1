/*
	sys/mac/time.cc
	---------------
*/

#include "Genie/FS/sys/mac/time.hh"

// vfs
#include "vfs/property.hh"
#include "vfs/node/types/property_file.hh"

// Genie
#include "Genie/FS/sys/mac/time/dls+gmt-delta.hh"


namespace Genie
{
	
	#define PROPERTY( prop )  &vfs::new_property, &prop##_params
	
	const vfs::fixed_mapping sys_mac_time_Mappings[] =
	{
		{ ".dls+gmt-delta", PROPERTY( sys_mac_time_dlsgmtdelta ) },
		{ "dls",            PROPERTY( sys_mac_time_dls         ) },
		{ "gmt-delta",      PROPERTY( sys_mac_time_gmtdelta    ) },
		
		{ ".~dls+gmt-delta", PROPERTY( sys_mac_time_dlsgmtdelta ) },
		{ ".~dls",           PROPERTY( sys_mac_time_dls         ) },
		{ ".~gmt-delta",     PROPERTY( sys_mac_time_gmtdelta    ) },
		
		{ NULL, NULL }
		
	};
	
}
