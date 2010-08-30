/*
	Genie/FS/sys/mac/event.cc
	-------------------------
*/

#include "Genie/FS/sys/mac/event.hh"

// Genie
#include "Genie/FS/sys/mac/event/post.hh"


namespace Genie
{
	
	const FSTree_Premapped::Mapping sys_mac_event_Mappings[] =
	{
		{ "post", &Premapped_Factory< sys_mac_event_post_Mappings > },
		
		{ NULL, NULL }
	};
	
}

